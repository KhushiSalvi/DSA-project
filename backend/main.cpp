#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <set>
#include <sstream>
#include <algorithm>
#include <limits>
#include <utility>

using namespace std;

// --- Hashing Tables for IUPAC Naming (v4) ---
map<int, string> ALKANE_NAME = {
    {1, "meth"}, {2, "eth"}, {3, "prop"}, {4, "but"},
    {5, "pent"}, {6, "hex"}, {7, "hept"}, {8, "oct"},
    {9, "non"}, {10, "dec"}
};
map<string, int> PARENTS_LEN = {
    {"meth", 1}, {"methane", 1},
    {"eth", 2}, {"ethane", 2},
    {"prop", 3}, {"propane", 3},
    {"but", 4}, {"butane", 4},
    {"pent", 5}, {"pentane", 5},
    {"hex", 6}, {"hexane", 6},
    {"hept", 7}, {"heptane", 7},
    {"oct", 8}, {"octane", 8},
    {"non", 9}, {"nonane", 9},
    {"dec", 10}, {"decane", 10}
};
map<int, string> SUBST_NAME = {
    {1, "methyl"}, {2, "ethyl"}, {3, "propyl"}, {4, "butyl"}
};
map<int, string> PREFIX_NAME = {
    {1, ""}, {2, "di"}, {3, "tri"}, {4, "tetra"}
};
map<string, string> GROUP_SUFFIX = {
    {"acid", "oic acid"}, {"aldehyde", "al"}, {"ketone", "one"},
    {"alcohol", "ol"}, {"alkyne", "yne"}, {"alkene", "ene"}
};
map<string, string> GROUP_PREFIX = {
    {"ketone", "oxo"}, {"alcohol", "hydroxy"}
};
map<string, int> GROUP_PRIORITY = {
    {"acid", 1}, {"aldehyde", 2}, {"ketone", 3}, {"alcohol", 4},
    {"alkyne", 5}, {"alkene", 6}, {"alkane", 7}
};

// --- Reverse Hashing Tables for Parsing ---
map<string, int> PARENT_LEN = {
    {"meth", 1}, {"eth", 2}, {"prop", 3}, {"but", 4}, {"pent", 5},
    {"hex", 6}, {"hept", 7}, {"oct", 8}, {"non", 9}, {"dec", 10}
};
map<string, int> SUBST_LEN = {
    {"methyl", 1}, {"ethyl", 2}, {"propyl", 3}, {"butyl", 4}
};
map<string, int> BOND_TYPE = {
    {"ene", 2}, {"yne", 3}
};
map<string, string> GROUP_TYPE = {
    {"ol", "O"}, {"one", "O"}, {"al", "O"}, {"oic acid", "O"}
};

struct Atom
{
    int id;
    string element;
    map<int, int> neighbors;
    string functional_group = "none";
    int chain_number = 0;
};

struct LocantSet
{
    vector<int> principal_groups;
    vector<int> double_bonds;
    vector<int> triple_bonds;
    vector<int> substituents;
    bool operator<(const LocantSet &other) const
    {
        if (principal_groups != other.principal_groups)
            return principal_groups < other.principal_groups;
        if (double_bonds != other.double_bonds)
            return double_bonds < other.double_bonds;
        if (triple_bonds != other.triple_bonds)
            return triple_bonds < other.triple_bonds;
        return substituents < other.substituents;
    }
    bool eneYneTieBreaker(const LocantSet &other) const
    {
        int this_yne = triple_bonds.empty() ? 1000 : triple_bonds[0];
        int other_yne = other.triple_bonds.empty() ? 1000 : other.triple_bonds[0];
        if (this_yne != other_yne)
            return this_yne < other_yne;
        int this_ene = double_bonds.empty() ? 1000 : double_bonds[0];
        int other_ene = other.double_bonds.empty() ? 1000 : other.double_bonds[0];
        return this_ene < other_ene;
    }
};
struct PathScore
{
    int priority_group_count = 0;
    int multiple_bond_count = 0;
    int length = 0;
    bool operator>(const PathScore &other) const
    {
        if (priority_group_count != other.priority_group_count)
            return priority_group_count > other.priority_group_count;
        if (multiple_bond_count != other.multiple_bond_count)
            return multiple_bond_count > other.multiple_bond_count;
        return length > other.length;
    }
};

class Molecule
{
public:
    map<int, Atom> atoms;
    string highest_priority_group = "alkane";
    set<int> principal_group_atoms;
    bool is_cyclic = false;
    int next_atom_id = 1;

    // Added members to support parsing/numbering of parent chain
    int main_chain_length = 0;
    string parent_stem = "";

    void addAtom(int id, string element)
    {
        if (atoms.find(id) == atoms.end())
        {
            atoms[id] = Atom{id, element};
            if (id >= next_atom_id)
                next_atom_id = id + 1;
        }
    }

    void addBond(int u, int v, int bondType)
    {
        if (atoms.find(u) == atoms.end() || atoms[u].element == "H")
            addAtom(u, "C");
        if (atoms.find(v) == atoms.end() || atoms[v].element == "H")
            addAtom(v, "C");
        atoms[u].neighbors[v] = bondType;
        atoms[v].neighbors[u] = bondType;
    }

 // --- Structure -> Name ---
    string getIUPACName()
    {
        if (atoms.empty())
            return "Empty Molecule";
        this->is_cyclic = isCyclic();
        findFunctionalGroups();
        vector<int> chain = findPrincipalChain();
        if (chain.empty())
            return "Error: No chain found.";

        if (chain.size() == 1)
        {
            if (highest_priority_group == "acid")
                return "methanoic acid";
            if (highest_priority_group == "aldehyde")
                return "methanal";
            return "methane";
        }

        pair<map<int, int>, map<string, vector<int>>> result = numberChainAndFindSubstituents(chain);
        map<int, int> numbering = result.first;
        map<string, vector<int>> substituents = result.second;
        return assembleName(chain, numbering, substituents);
    }
void buildFromName(string name)
    {
        atoms.clear();
        next_atom_id = 1;
        is_cyclic = false;
        main_chain_length = 0;
        this->parent_stem.clear();

        string parent_stem_local = "";
        string core_name = "";
        string prefix_name = "";
        int parent_len = 0;

        for (map<string, int>::const_iterator it = PARENTS_LEN.begin(); it != PARENTS_LEN.end(); ++it)
        {
            const string &stem = it->first;
            int len = it->second;
            size_t pos = name.find(stem);
            while (pos != string::npos)
            {
                bool prefix_ok = (pos == 0) || (name[pos - 1] == '-');
                size_t after = pos + stem.length();
                bool suffix_ok = (after == name.length()) || (name[after] == '-');
                if (prefix_ok && suffix_ok)
                {
                    parent_stem_local = stem;
                    parent_len = len;
                    prefix_name = name.substr(0, pos);
                    core_name = name.substr(pos);
                    break;
                }
                pos = name.find(stem, pos + 1);
            }
            if (parent_len > 0)
                break;
        }

        if (parent_len == 0)
        {
            cout << "Error: Could not parse parent stem." << endl;
            return;
        }

        this->parent_stem = parent_stem_local;

        bool cyclic = false;
        if (prefix_name.find("cyclo") != string::npos)
            cyclic = true;
        if (!core_name.empty() && core_name.find("cyclo") == 0)
            cyclic = true;
        is_cyclic = cyclic;

        buildMainChain(parent_len, cyclic);
        parseCore(core_name);
        parsePrefixes(prefix_name);
    }

void printAdjacencyList()
    {
        cout << "--- Generated Graph (Adjacency List) ---" << endl;
        for (map<int, Atom>::const_iterator it = atoms.begin(); it != atoms.end(); ++it)
        {
            int id = it->first;
            const Atom &atom = it->second;
            cout << "Atom " << id << " (" << atom.element << "): ";
            for (map<int, int>::const_iterator nit = atom.neighbors.begin(); nit != atom.neighbors.end(); ++nit)
            {
                int neighbor = nit->first;
                int type = nit->second;
                cout << "[-> " << neighbor << " (type " << type << ")] ";
            }
            cout << endl;
        }
    }
private:
void updatePriority(string group, int atom_id, int &current_highest_priority)
    {
        if (atoms[atom_id].functional_group == "none" || GROUP_PRIORITY[group] < GROUP_PRIORITY[atoms[atom_id].functional_group])
        {
            atoms[atom_id].functional_group = group;
        }
        if (GROUP_PRIORITY[group] < current_highest_priority)
        {
            current_highest_priority = GROUP_PRIORITY[group];
            highest_priority_group = group;
            principal_group_atoms.clear();
            principal_group_atoms.insert(atom_id);
        }
        else if (GROUP_PRIORITY[group] == current_highest_priority)
        {
            principal_group_atoms.insert(atom_id);
        }
    }
bool dfs_cycle_check(int u, int parent, set<int> &visited)
    {
        visited.insert(u);
        for (map<int, int>::const_iterator it = atoms[u].neighbors.begin(); it != atoms[u].neighbors.end(); ++it)
        {
            int v = it->first;
            if (v == parent)
                continue;
            if (visited.count(v))
                return true;
            if (dfs_cycle_check(v, u, visited))
                return true;
        }
        return false;
    }

    bool isCyclic()
    {
        set<int> visited;
        for (map<int, Atom>::const_iterator it = atoms.begin(); it != atoms.end(); ++it)
        {
            int id = it->first;
            if (!visited.count(id))
            {
                if (dfs_cycle_check(id, -1, visited))
                    return true;
            }
        }
        return false;
    }


    void findFunctionalGroups()
    {
        int current_highest_priority = GROUP_PRIORITY["alkane"];
        for (map<int, Atom>::iterator it = atoms.begin(); it != atoms.end(); ++it)
        {
            int id = it->first;
            Atom &atom = it->second;
            if (atom.element == "O")
            {
                int c_neighbor = -1;
                if (atom.neighbors.size() <= 2)
                {
                    for (map<int, int>::iterator nit = atom.neighbors.begin(); nit != atom.neighbors.end(); ++nit)
                    {
                        int neighbor_id = nit->first;
                        int bond_type = nit->second;
                        if (atoms[neighbor_id].element == "C" && bond_type == 1)
                        {
                            c_neighbor = neighbor_id;
                        }
                    }
                }
                if (c_neighbor != -1)
                {
                    updatePriority("alcohol", c_neighbor, current_highest_priority);
                }
            }
            else if (atom.element == "C")
            {
                bool has_double_bond_O = false;
                for (map<int, int>::iterator nit = atom.neighbors.begin(); nit != atom.neighbors.end(); ++nit)
                {
                    int neighbor_id = nit->first;
                    int bond_type = nit->second;
                    if (atoms[neighbor_id].element == "O" && bond_type == 2)
                    {
                        has_double_bond_O = true;
                        break;
                    }
                }
                if (has_double_bond_O)
                {
                    bool has_single_bond_O = false;
                    for (map<int, int>::iterator nit = atom.neighbors.begin(); nit != atom.neighbors.end(); ++nit)
                    {
                        int neighbor_id = nit->first;
                        int bond_type = nit->second;
                        if (atoms[neighbor_id].element == "O" && bond_type == 1)
                        {
                            has_single_bond_O = true;
                            break;
                        }
                    }
                    if (has_single_bond_O)
                    {
                        updatePriority("acid", id, current_highest_priority);
                    }
                    else
                    {
                        int c_neighbors = 0;
                        for (map<int, int>::iterator nit = atom.neighbors.begin(); nit != atom.neighbors.end(); ++nit)
                        {
                            int neighbor_id = nit->first;
                            if (atoms[neighbor_id].element == "C")
                                c_neighbors++;
                        }
                        if (c_neighbors <= 1)
                        {
                            updatePriority("aldehyde", id, current_highest_priority);
                        }
                        else
                        {
                            updatePriority("ketone", id, current_highest_priority);
                        }
                    }
                }
            }
        }

        if (current_highest_priority > GROUP_PRIORITY["alkyne"])
        {
            for (map<int, Atom>::iterator it = atoms.begin(); it != atoms.end(); ++it)
            {
                int id = it->first;
                Atom &atom = it->second;
                for (map<int, int>::iterator nit = atom.neighbors.begin(); nit != atom.neighbors.end(); ++nit)
                {
                    int bond_type = nit->second;
                    if (bond_type == 2)
                    {
                        updatePriority("alkene", id, current_highest_priority);
                    }
                    else if (bond_type == 3)
                    {
                        updatePriority("alkyne", id, current_highest_priority);
                    }
                }
            }
        }
    }
vector<int> findPrincipalChain()
    {
        if (is_cyclic)
        {
            vector<int> ring_atoms;
            for (map<int, Atom>::const_iterator it = atoms.begin(); it != atoms.end(); ++it)
            {
                const Atom &atom = it->second;
                if (atom.element == "C")
                    ring_atoms.push_back(it->first);
            }
            if (!ring_atoms.empty())
                return ring_atoms;
        }

        vector<vector<int>> all_paths;
        set<int> visited;
        set<int> endpoints;
        for (map<int, Atom>::const_iterator it = atoms.begin(); it != atoms.end(); ++it)
        {
            int id = it->first;
            const Atom &atom = it->second;
            if (atom.element == "C" && atom.neighbors.size() == 1)
            {
                endpoints.insert(id);
            }
        }
    }
};
