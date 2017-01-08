#include <vector>
#include <iostream>

#include <diy/mpi.hpp>
#include <diy/iexchange/master.hpp>
#include <diy/assigner.hpp>
#include <diy/serialization.hpp>

struct Block
{
    Block(): count(0)                   {}

    int   count;
};

void* create_block()                      { return new Block; }
void  destroy_block(void* b)              { delete static_cast<Block*>(b); }
void  save_block(const void* b,
                 diy::BinaryBuffer& bb)   { diy::save(bb, *static_cast<const Block*>(b)); }
void  load_block(void* b,
                 diy::BinaryBuffer& bb)   { diy::load(bb, *static_cast<Block*>(b)); }

// debug: test enqueue with original synchronous exchange
void enq(Block* b, const diy::Master::ProxyWithLink& cp)
{
    diy::Link* l = cp.link();

    // start with every block enqueueing its count the first time
    if (!b->count)
    {
        for (size_t i = 0; i < l->size(); ++i)
            cp.enqueue(cp.link()->target(i), b->count);
        b->count++;
    }
}

// debug: test dequeue with original synchronous exchange
void deq(Block* b, const diy::Master::ProxyWithLink& cp)
{
    diy::Link* l = cp.link();

    for (size_t i = 0; i < l->size(); ++i)
    {
        int gid = l->target(i).gid;
        if (cp.incoming(gid).size())
        {
            cp.dequeue(cp.link()->target(i).gid, b->count);
            b->count++;
            cp.enqueue(cp.link()->target(i), b->count);
        }
    }
}

// callback for asynchronous iexchange
bool foo(Block* b, const diy::Master::IProxyWithLink& icp)
{
    diy::Link* l = icp.link();
    int my_gid = icp.gid();

    // start with every block enqueueing its count the first time
    if (!b->count)
    {
        for (size_t i = 0; i < l->size(); ++i)
            icp.enqueue(l->target(i), b->count);
        b->count++;
    }
    fmt::print(stderr, "1: gid={} count={}\n", my_gid, b->count);

    // then dequeue/enqueue as long as there is something to do
    // TODO: dequeue does not clear incoming queue, should it?
    // does this pattern of looping while there is a nonzero q make sense?
#if 0
    size_t tot_q_size;
    while (1)
    {
        tot_q_size = 0;
        for (size_t i = 0; i < l->size(); ++i)
        {
            int nbr_gid = l->target(i).gid;
            tot_q_size += icp.incoming(nbr_gid).size();
            if (icp.incoming(nbr_gid).size())
            {
                icp.dequeue(nbr_gid, b->count);
                b->count++;
                icp.enqueue(l->target(i), b->count);
            }
        }

        if (!tot_q_size)
            break;
    }
#endif

    // then dequeue/enqueue
    fmt::print(stderr, "2: gid={} count={}\n", my_gid, b->count);
    for (size_t i = 0; i < l->size(); ++i)
    {
        int nbr_gid = l->target(i).gid;
        if (icp.incoming(nbr_gid).size())
        {
            fmt::print(stderr, "3: gid={}\n", my_gid);
            icp.dequeue(nbr_gid, b->count);
            b->count++;
            fmt::print(stderr, "4: gid={} count={}\n", my_gid, b->count);
            icp.enqueue(l->target(i), b->count);
            fmt::print(stderr, "5: gid={} count={}\n", my_gid, b->count);
        }
    }

    // flip a coin to decide whether to be done
    int done = rand() % 2;
    // icp.collectives()->clear();
    // icp.all_reduce(done, std::plus<int>());

    fmt::print(stderr, "returning: gid={} count={} done={}\n", my_gid, b->count, done);

    // return (tot_q_size ? false : true);
    return (true);                           // TODO: hard code all done
}

int main(int argc, char* argv[])
{
    diy::create_logger("trace");

    diy::mpi::environment     env(argc, argv);
    diy::mpi::communicator    world;

    int                       nblocks = 4 * world.size();

    diy::FileStorage          storage("./DIY.XXXXXX");

    diy::Master               master(world,
                                     1,
                                     -1,
                                     &create_block,
                                     &destroy_block,
                                     &storage,
                                     &save_block,
                                     &load_block);

    srand(time(NULL));

    diy::RoundRobinAssigner   assigner(world.size(), nblocks);

    // this example creates a linear chain of blocks
    std::vector<int> gids;                     // global ids of local blocks
    assigner.local_gids(world.rank(), gids);   // get the gids of local blocks
    for (size_t i = 0; i < gids.size(); ++i)   // for the local blocks in this processor
    {
        int gid = gids[i];

        diy::Link*   link = new diy::Link;   // link is this block's neighborhood
        diy::BlockID neighbor;               // one neighbor in the neighborhood
        if (gid < nblocks - 1)               // all but the last block in the global domain
        {
            neighbor.gid  = gid + 1;                     // gid of the neighbor block
            neighbor.proc = assigner.rank(neighbor.gid); // process of the neighbor block
            link->add_neighbor(neighbor);                // add the neighbor block to the link
        }
        if (gid > 0)                         // all but the first block in the global domain
        {
            neighbor.gid  = gid - 1;
            neighbor.proc = assigner.rank(neighbor.gid);
            link->add_neighbor(neighbor);
        }

        master.add(gid, new Block, link);    // add the current local block to the master
    }

#if 0
    // test synchronous version
    master.foreach(&enq);
    master.exchange();
    master.foreach(&deq);
#else
    // dequeue, enqueue, exchange all in one nonblocking routine
    master.iexchange(&foo);
#endif

    if (world.rank() == 0)
        fmt::print(stderr,
                   "Total iterations: {}\n", master.block<Block>(master.loaded_block())->count);

}
