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
        if (endpoints.empty() && !atoms.empty())
        {
            for (map<int, Atom>::const_iterator it = atoms.begin(); it != atoms.end(); ++it)
            {
                int id = it->first;
                const Atom &atom = it->second;
                if (atom.element == "C")
                {
                    endpoints.insert(id);
                    break;
                }
            }
        }
        for (int start_node : endpoints)
        {
            visited.clear();
            dfs_find_paths(start_node, {start_node}, visited, all_paths);
        }
        if (all_paths.empty() && atoms.size() == 1)
        {
            for (map<int, Atom>::const_iterator it = atoms.begin(); it != atoms.end(); ++it)
            {
                const Atom &atom = it->second;
                if (atom.element == "C")
                    return {it->first};
            }
        }
        PathScore best_score;
        vector<int> best_path;
        for (const auto &path : all_paths)
        {
            PathScore current_score = scorePath(path);
            if (current_score > best_score)
            {
                best_score = current_score;
                best_path = path;
            }
        }
        return best_path;
    }

    void dfs_find_paths(int u, vector<int> current_path, set<int> &visited, vector<vector<int>> &all_paths)
    {
        visited.insert(u);
        bool at_end = true;
        for (map<int, int>::const_iterator it = atoms[u].neighbors.begin(); it != atoms[u].neighbors.end(); ++it)
        {
            int v = it->first;
            if (atoms[v].element == "C" && visited.find(v) == visited.end())
            {
                at_end = false;
                vector<int> new_path = current_path;
                new_path.push_back(v);
                dfs_find_paths(v, new_path, visited, all_paths);
            }
        }
        if (at_end)
            all_paths.push_back(current_path);
        visited.erase(u);
    }


    PathScore scorePath(const vector<int> &path)
    {
        PathScore score;
        score.length = path.size();
        for (int u : path)
        {
            if (principal_group_atoms.count(u))
            {
                score.priority_group_count++;
            }
        }
        for (size_t i = 0; i < path.size() - 1; ++i)
        {
            int u = path[i];
            int v = path[i + 1];
            if (atoms[u].neighbors.count(v) && atoms[u].neighbors.at(v) > 1)
            {
                score.multiple_bond_count++;
            }
        }
        return score;
    }
  pair<map<int, int>, map<string, vector<int>>> numberChainAndFindSubstituents(const vector<int> &chain)
    {
        map<int, int> numberingA, numberingB;
        for (size_t i = 0; i < chain.size(); ++i)
        {
            numberingA[chain[i]] = i + 1;
        }
        for (size_t i = 0; i < chain.size(); ++i)
        {
            numberingB[chain[chain.size() - 1 - i]] = i + 1;
        }
        map<string, vector<int>> subsA = getSubstituents(chain, numberingA);
        map<string, vector<int>> subsB = getSubstituents(chain, numberingB);
        LocantSet locantsA = getLocantList(chain, numberingA, subsA);
        LocantSet locantsB = getLocantList(chain, numberingB, subsB);
        if (locantsA < locantsB)
        {
            return make_pair(numberingA, subsA);
        }
        else if (locantsB < locantsA)
        {
            return make_pair(numberingB, subsB);
        }
        else
        {
            if (locantsA.eneYneTieBreaker(locantsB))
            {
                return make_pair(numberingA, subsA);
            }
            else if (locantsB.eneYneTieBreaker(locantsA))
            {
                return make_pair(numberingB, subsB);
            }
            string subsA_alpha = getAlphabeticalSubList(subsA);
            string subsB_alpha = getAlphabeticalSubList(subsB);
            if (subsA_alpha <= subsB_alpha)
            {
                return make_pair(numberingA, subsA);
            }
            else
            {
                return make_pair(numberingB, subsB);
            }
        }
    }
string getAlphabeticalSubList(const map<string, vector<int>> &substituents)
    {
        stringstream ss;
        for (map<string, vector<int>>::const_iterator it = substituents.begin(); it != substituents.end(); ++it)
        {
            const string &name = it->first;
            const vector<int> &locs = it->second;
            for (int loc : locs)
                ss << loc << "-" << name << "-";
        }
        return ss.str();
    }
LocantSet getLocantList(const vector<int> &chain, const map<int, int> &numbering, const map<string, vector<int>> &substituents)
    {
        LocantSet locants;
        for (int atom_id : principal_group_atoms)
        {
            if (numbering.count(atom_id))
            {
                locants.principal_groups.push_back(numbering.at(atom_id));
            }
        }
        sort(locants.principal_groups.begin(), locants.principal_groups.end());
        set<int> main_chain_nodes(chain.begin(), chain.end());
        for (map<int, int>::const_iterator it = numbering.begin(); it != numbering.end(); ++it)
        {
            int atom_id = it->first;
            int num = it->second;
            for (map<int, int>::const_iterator nit = atoms[atom_id].neighbors.begin(); nit != atoms[atom_id].neighbors.end(); ++nit)
            {
                int neighbor_id = nit->first;
                int bond_type = nit->second;
                if (bond_type > 1 && main_chain_nodes.count(neighbor_id) && numbering.count(neighbor_id) && numbering.at(neighbor_id) > num)
                {
                    if (bond_type == 2)
                        locants.double_bonds.push_back(num);
                    if (bond_type == 3)
                        locants.triple_bonds.push_back(num);
                }
            }
        }
        sort(locants.double_bonds.begin(), locants.double_bonds.end());
        sort(locants.triple_bonds.begin(), locants.triple_bonds.end());
        for (map<string, vector<int>>::const_iterator it = substituents.begin(); it != substituents.end(); ++it)
        {
            const vector<int> &locs = it->second;
            for (int l : locs)
                locants.substituents.push_back(l);
        }
        sort(locants.substituents.begin(), locants.substituents.end());
        return locants;
    }

    map<string, vector<int>> getSubstituents(const vector<int> &chain, const map<int, int> &numbering)
    {
        map<string, vector<int>> substituents;
        set<int> main_chain_nodes(chain.begin(), chain.end());
        set<int> visited;
        for (int chain_atom_id : chain)
        {
            int location = numbering.at(chain_atom_id);
            for (map<int, int>::const_iterator it = atoms[chain_atom_id].neighbors.begin(); it != atoms[chain_atom_id].neighbors.end(); ++it)
            {
                int neighbor_id = it->first;
                if (main_chain_nodes.find(neighbor_id) == main_chain_nodes.end() &&
                    visited.find(neighbor_id) == visited.end() &&
                    atoms[neighbor_id].element == "C")
                {
                    int size = dfs_subst_size(neighbor_id, visited, main_chain_nodes);
                    string name = SUBST_NAME.count(size) ? SUBST_NAME[size] : "unknown";
                    substituents[name].push_back(location);
                }
            }
            string group = atoms[chain_atom_id].functional_group;
            if (group != "none" && group != highest_priority_group && GROUP_PREFIX.count(group))
            {
                substituents[GROUP_PREFIX.at(group)].push_back(location);
            }
        }
        return substituents;
    }
    int dfs_subst_size(int u, set<int> &visited, const set<int> &main_chain_nodes)
    {
        visited.insert(u);
        int count = 1;
        for (map<int, int>::const_iterator it = atoms[u].neighbors.begin(); it != atoms[u].neighbors.end(); ++it)
        {
            int v = it->first;
            if (visited.find(v) == visited.end() &&
                main_chain_nodes.find(v) == main_chain_nodes.end() &&
                atoms[v].element == "C")
            {
                count += dfs_subst_size(v, visited, main_chain_nodes);
            }
        }
        return count;
    }
string assembleName(const vector<int> &chain, const map<int, int> &numbering, const map<string, vector<int>> &substituents)
    {
        stringstream ss;
        vector<string> sub_names;
        for (map<string, vector<int>>::const_iterator it = substituents.begin(); it != substituents.end(); ++it)
        {
            sub_names.push_back(it->first);
        }
        sort(sub_names.begin(), sub_names.end());

        for (const string &name : sub_names)
        {
            auto locs = substituents.at(name);
            sort(locs.begin(), locs.end());
            for (size_t i = 0; i < locs.size(); ++i)
            {
                ss << locs[i] << (i < locs.size() - 1 ? "," : "-");
            }
            ss << PREFIX_NAME[locs.size()] << name << "-";
        }

        string prefix = is_cyclic ? "cyclo" : "";
        string parent = prefix + ALKANE_NAME[chain.size()];

        map<int, int> double_bonds, triple_bonds;
        for (map<int, int>::const_iterator it = numbering.begin(); it != numbering.end(); ++it)
        {
            int u = it->first;
            int num = it->second;
            for (map<int, int>::const_iterator nit = atoms[u].neighbors.begin(); nit != atoms[u].neighbors.end(); ++nit)
            {
                int v = nit->first;
                int bond = nit->second;
                if (numbering.count(v) && numbering.at(v) > num)
                {
                    if (bond == 2)
                        double_bonds[num] = 2;
                    if (bond == 3)
                        triple_bonds[num] = 3;
                }
            }
        }

        vector<int> db_locs, tb_locs;
        for (map<int, int>::const_iterator it = double_bonds.begin(); it != double_bonds.end(); ++it)
            db_locs.push_back(it->first);
        for (map<int, int>::const_iterator it = triple_bonds.begin(); it != triple_bonds.end(); ++it)
            tb_locs.push_back(it->first);
        sort(db_locs.begin(), db_locs.end());
        sort(tb_locs.begin(), tb_locs.end());

        string multiple_bond_suffix = "";}
if (!tb_locs.empty())
        {
            multiple_bond_suffix += "-";
            for (size_t i = 0; i < tb_locs.size(); ++i)
            {
                multiple_bond_suffix += to_string(tb_locs[i]);
                if (i < tb_locs.size() - 1)
                    multiple_bond_suffix += ",";
            }
            multiple_bond_suffix += "-yne";
        }
        if (!db_locs.empty())
        {
            if (!multiple_bond_suffix.empty())
                multiple_bond_suffix += "-";
            multiple_bond_suffix += "-";
            for (size_t i = 0; i < db_locs.size(); ++i)
            {
                multiple_bond_suffix += to_string(db_locs[i]);
                if (i < db_locs.size() - 1)
                    multiple_bond_suffix += ",";
            }
            multiple_bond_suffix += "-ene";
        }

        if (highest_priority_group != "alkene" && highest_priority_group != "alkyne")
        {
            vector<int> group_locants;
            for (int atom_id : principal_group_atoms)
            {
                if (numbering.count(atom_id))
                    group_locants.push_back(numbering.at(atom_id));
            }
            sort(group_locants.begin(), group_locants.end());
            if (!group_locants.empty())
            {
                if (!multiple_bond_suffix.empty())
                    multiple_bond_suffix += "-";
                for (size_t i = 0; i < group_locants.size(); ++i)
                {
                    multiple_bond_suffix += to_string(group_locants[i]);
                    if (i < group_locants.size() - 1)
                        multiple_bond_suffix += ",";
                }
                multiple_bond_suffix += "-" + GROUP_SUFFIX[highest_priority_group];
            }
        }

        return ss.str() + parent + multiple_bond_suffix;

     vector<string> splitString(const string &s, const string &delimiters)
    {
        vector<string> tokens;
        size_t start = 0, end = 0;
        while ((end = s.find_first_of(delimiters, start)) != string::npos)
        {
            if (end != start)
                tokens.push_back(s.substr(start, end - start));
            start = end + 1;
        }
        if (start < s.length())
            tokens.push_back(s.substr(start));
        return tokens;
    }

    // build main chain; if cyclic is true, close ring (connect last to first)
    void buildMainChain(int len, bool cyclic = false)
    {
        main_chain_length = len;
        for (int i = 1; i <= len; i++)
        {
            addAtom(i, "C");
        }
        for (int i = 1; i < len; i++)
        {
            addBond(i, i + 1, 1);
        }
        if (cyclic && len >= 3)
        {
            addBond(len, 1, 1);
        }
    }


 void parseCore(string core_name)
    {
        if (core_name.empty())
            return;
        vector<string> tokens = splitString(core_name, "-");
        if (tokens.empty())
            return;

        size_t idx = 0;
        // skip explicit parent stem token if present
        if (!this->parent_stem.empty() && tokens[0] == this->parent_stem)
        {
            idx = 1;
        }
        else if (tokens[0].find("cyclo") == 0)
        {
            if (tokens.size() > 1 && !this->parent_stem.empty() && tokens[1] == this->parent_stem)
                idx = 2;
            else
                idx = 1;
        }

        vector<int> current_locants;
        for (; idx < tokens.size(); ++idx)
        {
            string token = tokens[idx];
            if (token.empty())
                continue;

            // locant list like "1,3,5"
            if (isdigit(token[0]))
            {
                current_locants.clear();
                vector<string> parts = splitString(token, ",");
                for (const string &p : parts)
                {
                    if (!p.empty() && isdigit(p[0]))
                    {
                        try
                        {
                            int v = stoi(p);
                            current_locants.push_back(v);
                        }
                        catch (...)
                        {
                        }
                    }
                }
                continue;
            }

            // multiplicative prefixes: ignore if locants present
            if (token == "di" || token == "tri" || token == "tetra")
            {
                continue;
            }

            // bond type (ene,yne)
            if (BOND_TYPE.count(token))
            {
                int type = BOND_TYPE.at(token);
                if (!current_locants.empty())
                {
                    for (int loc : current_locants)
                    {
                        int a = loc;
                        int b = loc + 1;
                        if (this->is_cyclic && this->main_chain_length > 0)
                        {
                            if (b > this->main_chain_length)
                                b = 1;
                        }
                        if (atoms.count(a) && atoms.count(b))
                        {
                            if (!atoms[a].neighbors.count(b) || atoms[a].neighbors[b] < type)
                            {
                                addBond(a, b, type);
                            }
                        }
                    }
                }
                current_locants.clear();
                continue;
            }

            // functional groups like ol, one, al, oic acid
            if (GROUP_TYPE.count(token))
            {
                string element = GROUP_TYPE.at(token);
                if (!current_locants.empty())
                {
                    for (int loc : current_locants)
                    {
                        int new_id = next_atom_id++;
                        addAtom(new_id, element);
                        if (token == "one" || token == "al" || token == "oic acid")
                        {
                            addBond(loc, new_id, 2);
                        }
                        else
                        {
                            addBond(loc, new_id, 1);
                        }
                    }
                    current_locants.clear();
                }
                continue;
            }

            // ignore unknown tokens (e.g., stray "cyclo" or unexpected text)
            current_locants.clear();
        }
    }

 void parsePrefixes(string prefix_name)
    {
        vector<string> tokens = splitString(prefix_name, "-");
        vector<int> locants;
        for (const string &token : tokens)
        {
            if (token.empty())
                continue;
            if (isdigit(token[0]))
            {
                locants.clear();
                vector<string> loc_list = splitString(token, ",");
                for (const string &loc : loc_list)
                {
                    locants.push_back(stoi(loc));
                }
            }
            else
            {
                string base_name = "";
                if (token.compare(0, 2, "di") == 0)
                {
                    base_name = token.substr(2);
                }
                else if (token.compare(0, 3, "tri") == 0)
                {
                    base_name = token.substr(3);
                }
                else if (token.compare(0, 5, "tetra") == 0)
                {
                    base_name = token.substr(5);
                }
                else
                {
                    base_name = token;
                }

                if (SUBST_LEN.count(base_name))
                {
                    buildSubstituent(locants, base_name, locants.size());
                }
                else if (base_name == "oxo" || base_name == "hydroxy")
                {
                    for (int loc : locants)
                    {
                        int new_id = next_atom_id++;
                        addAtom(new_id, "O");
                        if (base_name == "oxo")
                            addBond(loc, new_id, 2);
                        else if (base_name == "hydroxy")
                            addBond(loc, new_id, 1);
                    }
                }
                locants.clear();
            }
        }
    }
 void buildSubstituent(vector<int> locants, string base_name, int /*count */)
    {
        if (!SUBST_LEN.count(base_name))
            return;
        int size = SUBST_LEN.at(base_name);
        for (int loc : locants)
        {
            int prev_atom = loc;
            for (int i = 0; i < size; i++)
            {
                int new_atom_id = next_atom_id++;
                addAtom(new_atom_id, "C");
                addBond(prev_atom, new_atom_id, 1);
                prev_atom = new_atom_id;
            }
        }
    }
};

    // --- Main function with menu ---
int main()
{
    Molecule mol;
    int choice = 0;

    cout << "========================================" << endl;
    cout << "  IUPAC Name <-> Structure Converter" << endl;
    cout << "========================================" << endl;

    while (true)
    {
        cout << "\nSelect an option:" << endl;
        cout << "  1. Convert Structure (Graph) to IUPAC Name" << endl;
        cout << "  2. Convert IUPAC Name to Structure (Graph)" << endl;
        cout << "  3. Exit" << endl;
        cout << "Enter choice: ";

        if (!(cin >> choice))
        {
            cout << "Invalid input. Please enter a number." << endl;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }

        if (choice == 1)
        {
            mol.atoms.clear();
            mol.next_atom_id = 1;
            string type;
            cout << "\nEnter atoms (e.g., 'atom 1 C') or 'atom 2 O'" << endl;
            cout << "Enter bonds (e.g., 'bond 1 2 1') for single bond, 2 for double, 3 for triple" << endl;
            cout << "Type 'name' to finish." << endl
                 << endl;

            while (cin >> type && type != "name")
            {
                if (type == "atom")
                {
                    int id;
                    string element;
                    if (!(cin >> id >> element))
                    {
                        cout << "Error reading atom data." << endl;
                        cin.clear();
                        cin.ignore(numeric_limits<streamsize>::max(), '\n');
                        continue;
                    }
                    mol.addAtom(id, element);
                }
                else if (type == "bond")
                {
                    int u, v, t;
                    if (!(cin >> u >> v >> t))
                    {
                        cout << "Error reading bond data." << endl;
                        cin.clear();
                        cin.ignore(numeric_limits<streamsize>::max(), '\n');
                        continue;
                    }
                    mol.addBond(u, v, t);
                }
                else
                {
                    cout << "Invalid command: " << type << ". Use 'atom' or 'bond'." << endl;
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                }
            }
            cout << "\n--- Generating IUPAC Name ---" << endl;
            string iupac_name = mol.getIUPACName();
            cout << "\n---------------------------------" << endl;
            cout << "Final IUPAC Name: " << iupac_name << endl;
            cout << "---------------------------------" << endl;
        }
        else if (choice == 2)
        {
            mol.atoms.clear();
            mol.next_atom_id = 1;
            mol.is_cyclic = false;
            mol.main_chain_length = 0;
            mol.parent_stem.clear();
            string name = "";
            cout << "\nEnter IUPAC Name (e.g., 4-methylpent-2-en-1-ol or cyclohex-1,3,5-tri-ene):" << endl;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            getline(cin, name);

            // normalize: remove spaces
            name.erase(remove(name.begin(), name.end(), ' '), name.end());
            // convert to lowercase for robustness
            transform(name.begin(), name.end(), name.begin(), ::tolower);

            cout << "\n--- Building Graph from Name ---" << endl;
            mol.buildFromName(name);
            mol.printAdjacencyList();
            cout << "---------------------------------" << endl;
        }
        else if (choice == 3)
        {
            cout << "Exiting." << endl;
            break;
        }
        else
        {
            cout << "Invalid choice. Please enter 1, 2, or 3." << endl;
        }
    }
    return 0;
}
        

