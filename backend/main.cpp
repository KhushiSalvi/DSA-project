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