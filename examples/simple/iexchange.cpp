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

bool foo(Block* b, const diy::Master::IProxyWithLink& icp)
{
    diy::Link* l = icp.link();

    // start with every block enqueueing its count the first time
    if (!b->count)
    {
        for (size_t i = 0; i < l->size(); ++i)
        {
            fmt::print(stderr, "enq: gid={} count={}\n", icp.gid(), b->count);
            icp.enqueue(icp.link()->target(i), b->count);
            b->count++;
        }
    }

    // then dequeue/enqueue as long as there is something to do
    size_t tot_q_size;
    while (1)
    {
        tot_q_size = 0;
        for (size_t i = 0; i < l->size(); ++i)
        {
            tot_q_size += icp.incoming(i).size();
            if (icp.incoming(i).size())
            {
                icp.dequeue(icp.link()->target(i).gid, b->count);
                b->count++;
                fmt::print(stderr, "deq: gid={} count={}\n", icp.gid(), b->count);
                icp.enqueue(icp.link()->target(i), b->count);
            }
        }

        if (!tot_q_size)
            break;
    }

    // flip a coin to decide whether to be done
    int done = rand() % 2;
    // icp.collectives()->clear();
    // icp.all_reduce(done, std::plus<int>());

    fmt::print(stderr, "returning: gid={} count={} done={}\n", icp.gid(), b->count, done);

    // return (tot_q_size ? false : true);
    return (true);                           // TODO: hard code all done
}

int main(int argc, char* argv[])
{
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

    // for (int gid = 0; gid < nblocks; ++gid)
    //     if (assigner.rank(gid) == world.rank())
    //         master.add(gid, new Block, new diy::Link);

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

    // dequeue, enqueue, exchange all in one nonblocking routine
    master.iexchange(&foo);

    if (world.rank() == 0)
        fmt::print(stderr,
                   "Total iterations: {}\n", master.block<Block>(master.loaded_block())->count);

}
