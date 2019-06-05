#include <iostream>
#include <vector>
#include "../lib/atom.hpp"
#include "../lib/hydrogenic.hpp"
#include "../lib/constants.hpp"
#include "../vendor/aixlog/aixlog.hpp"

using namespace std;

int main(int argc, char **argv)
{
    AixLog::Log::init({make_shared<AixLog::SinkFile>(AixLog::Severity::trace, AixLog::Type::normal, "arbitrary_state.log", "#message"),
                       make_shared<AixLog::SinkFile>(AixLog::Severity::trace, AixLog::Type::special, "arbitrary_state.err")});

    LOG(INFO) << "Starting arbitrary_state...\n";

    // Parameters
    if (argc < 5)
    {
        cout << "Wrong number of arguments\n";
        return -1;
    }

    int k;
    double Z = stod(argv[1]);
    int l = stoi(argv[2]);
    bool s = stoi(argv[3]) != 0;
    double E = stod(argv[4]);
    double A = argc > 5 ? stod(argv[5]) : -1;

    DiracAtom da = DiracAtom(Z, 1, A);
    E += da.getRestE();
    qnumSchro2Dirac(l, s, k);

    pair<int, int> glim;

    try
    {
        glim = da.gridLimits(E, k);
    }
    catch (AtomErrorCode acode)
    {
        LOG(ERROR) << SPECIAL << "FAILED: AtomErrorCode = " << acode << "\n";
    }
    DiracState ds = DiracState(da.getrc(), da.getdx(), glim.first, glim.second);
    TurningPoint tp;
    ds.E = E;
    ds.k = k;
    ds.V = da.getV(ds.grid);
    da.integrateState(ds, tp);
    ds.continuify(tp);
    ds.findNodes();
    LOG(TRACE) << "State converged, nodes = " << ds.nodes << ", nodesQ = " << ds.nodesQ << "\n";

    int N = ds.grid.size();
    for (int i = 0; i < N; ++i)
    {
        cout << ds.grid[i] << '\t' << ds.P[i] << '\t' << ds.Q[i] << '\n';
    }
}