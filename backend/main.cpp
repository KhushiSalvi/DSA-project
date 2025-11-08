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

