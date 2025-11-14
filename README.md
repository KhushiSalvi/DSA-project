# IUPAC Name ↔ Structure Converter

A C++ based bidirectional chemical naming engine that converts molecular structure graphs into correct IUPAC names. It also builds chemical structures from IUPAC names using dynamic parsing, graph algorithms, and functional group logic.


## Features

### Structure → Name
- Automatically detects the longest principal carbon chain
- Correctly identifies and prioritizes functional groups (alcohol, aldehyde, ketone, acid, alkene, alkyne)
- Smart numbering from both ends (picks the lexicographically smallest locant set)
- Dynamically detects substituents (methyl, ethyl, propyl, etc.)
- Generates the full IUPAC name including:
  - Substituents
  - Multiplicative prefixes (di, tri, tetra)
  - Double and triple bonds
  - Functional group suffixes
- Detects cycles


### Name → Structure
- Parses complex IUPAC names such as:
  - 4-methylpent-2-en-1-ol
  - cyclohex-1,3-dien-5-one
- Builds the carbon skeleton dynamically
- Adds double and triple bonds using locants
- Adds functional groups (oxo, hydroxy, aldehyde, acid, etc.)
- Adds substituent carbon chains
- Outputs an adjacency list of the final molecule


## Dynamic Parsing Logic
- Breaks down names using hyphens and commas
- Detects the parent chain length with hashing tables
- Automatically recognizes cyclo- prefixes
- Places bonds intelligently using locants


## Clean Menu Interface

1. Convert Structure → Name  
2. Convert Name → Structure  
3. Exit


# How the System Works

## 1. Hash Tables
The program uses several map<> hashing tables:
- Parent chain stems: meth, eth, prop, etc.
- Functional group suffixes: ol, al, one, oic acid
- Substituent names: methyl, ethyl, etc.
- Priority table for IUPAC rules

These allow for instant lookup and make parsing efficient.


## 2. Graph Representation
Each atom is represented as:

struct Atom {

    int id;
    string element;
    map<int,int> neighbors; 
    // neighbor -> bond type
};


This structure allows:
- Easy depth-first search (DFS)  
- Bond type tracking  
- Dynamic chain building


## 3. Structure → Name Flow

findFunctionalGroups()   
→ findPrincipalChain()   
→ numberChainAndFindSubstituents()   
→ assembleName()   


## 4. Name → Structure Flow

buildMainChain()   
→ parseCore()   
→ parsePrefixes()   
→ printAdjacencyList()   


# Example 1: Structure → Name

### Input:

atom 1 C  
atom 2 C  
atom 3 C  
bond 1 2 1  
bond 2 3 1  
name  

### Internal Steps:
- Chain length = 3 → prop-
- No functional groups
- No substituents
- No double or triple bonds

### Output:

propane  


# Example 2: Name → Structure

### Input:

4-methylpent-2-ene  

### Internal Processing:
- Parent = pent → 5-carbon chain  
- Double bond at position 2 → C2=C3  
- Substituent = methyl at C4  

### Output Adjacency List:

Atom 1 (C): [->2(type 1)]  
Atom 2 (C): [->1(type 1), ->3 (type 2)]  
Atom 3 (C): [->2 (type 2), ->4(type 1)]  
Atom 4 (C): [->3(type 1), ->6(type 1),->5(type 1)]  
Atom 5 (C): [->4(type 1)]  
Atom 6 (C): [->4(type 1)]   


# Major Functions Explained

### addAtom()
Creates a new carbon or oxygen atom.

### addBond()
Adds a single, double, or triple bond.

### findFunctionalGroups()
Detects:
- Alcohols
- Aldehydes
- Acids
- Ketones
- Alkenes
- Alkynes

### findPrincipalChain()
Uses DFS and scoring based on:
- Functional group priority
- Number of multiple bonds
- Chain length

### numberChainAndFindSubstituents()
Tests numbering from both ends and selects the correct IUPAC numbering.

### assembleName()
Builds the final IUPAC name:

substituents + parent chain + double or triple bonds + functional group

### buildFromName()
Main reverse parser that converts IUPAC name to molecule.

### parseCore()
Reads:
- Locants
- Double or triple bond indicators
- Functional group suffixes

### parsePrefixes()
Handles:
- Substituent chains (methyl, ethyl)
- Functional group prefixes (hydroxy, oxo)

### printAdjacencyList()
Outputs the molecule graph.


# Important Notes
- Supports most common IUPAC naming patterns
- Substituent detection is fully dynamic
- Uses chemical priority rules
- Supports cyclic structures such as:
  - cyclohexane
  - cyclopentanol
  - cyclohex-1-ene

# Design Choices

## Why We Used These Structures & Algorithms (and Not Others)


# 1. Why We Use a Graph (Adjacency List)

### Graph is the natural representation of molecules:
- Atoms = nodes
- Bonds = edges
- Bond order = edge label

Molecules have branches, chains, rings, and multiple bonds. These can only be modeled properly with graphs.

### Why NOT arrays or linear lists?
- Cannot store branching.
- Cannot represent rings or cross-links.

### Why NOT adjacency matrix?
- Wasteful: molecule graphs are sparse.
- O(n²) space complexity.

Conclusion:  
Adjacency-list graph is ideal for molecular structures.


# 2. Why map<int, Atom> Instead of vector<Atom> or unordered_map<int,Atom>

### map ensures:
- Deterministic ordering (important in IUPAC rules)
- Safe and efficient insertion of atoms with arbitrary IDs
- Stable ordering for lexicographic comparisons

### Why NOT vector?
- Inserting atoms breaks indexing
- Requires contiguous storage
- Dynamic insertion may reallocate and invalidate references

### Why NOT unordered_map?
- Random iteration order leads to inconsistent naming output

Conclusion:  
map guarantees predictable behavior required for IUPAC correctness.


# 3. Why Atom.neighbors Is map<int,int>

### We need:
- Fast neighbor lookup
- Precise bond type representation
- Ordering guarantees

### Why NOT vector-based neighbors?
- Cannot track bond types well
- Poor for searching specific neighbors

### Why NOT adjacency matrix?
- Sparse molecules waste space

Conclusion:  
map<int,int> is a clean, efficient labeled-edge structure.


# 4. Why DFS for Principal Chain Detection?

### DFS allows:
- Enumerating all possible carbon paths
- Detecting cycles
- Backtracking for correct IUPAC chain selection

### Why NOT BFS?
- BFS only gives the shortest path first
- Cannot find all longest chains
- Cannot evaluate functional group priority paths

Conclusion:  
DFS is the only traversal suitable for chain detection.


# 5. Why Use a Scoring System For Parent Chain Selection?

IUPAC parent chain selection prioritizes:

1. Functional group carbon inclusion  
2. Multiple bonds  
3. Maximum chain length  

Only a multilevel scoring system handles this correctly.

### Why NOT “Just pick the longest chain”?
- This fails for:
  - Alcohols
  - Aldehydes
  - Carboxylic acids
  - Double and triple bonds

Conclusion:  
Scoring ensures correct implementation of IUPAC chain priorities.


# 6. Why Bidirectional Numbering?

IUPAC mandates:

> Choose the numbering that gives the lowest set of locants.

Thus:
- Number left to right
- Number right to left
- Generate locant sets
- Compare lexicographically
- Choose the best numbering

### Why NOT one-direction numbering?
- One-direction numbering breaks:
  - Double bond prioritization
  - Substituent locant minimization
  - Tie-break rules

Conclusion:  
Bidirectional numbering is required for valid IUPAC names.


# 7. Why Use Hash Tables (map<string,int>)

### Perfect for:
- Parent stems (meth, eth, prop)
- Group suffixes (ol, al, one)
- Priority mappings
- Substituent names

### Why NOT hardcoding via if-else?
- Extremely messy
- Hard to add new functional groups


Conclusion:  
Hash tables provide a clean, scalable, and fast lookup method.

# Conclusion

- Provides a fully working C++ engine for IUPAC name ↔ structure conversion

- Uses graph theory, DFS, and hash tables for accurate chemical representation

- Correctly handles functional groups, substituents, multiple bonds, and cyclic structures

- Follows IUPAC rules for numbering, priority, and naming logic

- Compact, reliable, and easy to extend for more advanced chemical tools and features

# Team Contributions 

## Teenu Kumari

- Added interactive build option (in main menu)

- Implemented substituent chain builder (buildSubstituent())

- Implemented locant extraction (getLocantList())

- Implemented locant-based substituent comparison (getAlphabeticalSubList())

- Enhanced naming engine (assembleName())

- Added functional-group priority logic (updatePriority())

- Improved principal chain detection (findPrincipalChain(), numberChainAndFindSubstituents())

- Multiple bug fixes (main.cpp, molecule.cpp)

## Khushi Salvi

- Added main function with menu (main())

- Added function to build parent chain (buildMainChain())

- Added DFS path search (dfs_find_paths())

- Added substituent extraction (getSubstituents())

- Implemented functional group finder (findFunctionalGroups())

- Added adjacency list printer (printAdjacencyList())

- Initial code + hash tables (ALKANE_NAME, SUBST_NAME, etc.)

## Riddhi Jain

- Created core Molecule class (class Molecule)

- Implemented chain numbering logic (determineOptimalNumbering())

- Enhanced principal chain logic (findPrincipalChain(), scorePath())

- Added alphabetical sorting for substituents (getAlphabeticalSubList())

- Added cyclic structure check (isCyclic(), dfs_cycle_check())

- Fixed bugs in molecule & main (bug fixes)

- Helped refine path scoring (scorePath())

## Mahi Gupta

- Implemented core parser for IUPAC names (parseCore())

- Added substituent-size DFS (dfs_subst_size())

- Improved path scoring (scorePath())

- Updated functional-group detection (findFunctionalGroups())

- Implemented large parts of name-to-structure builder (buildFromName())

- General debugging & fixes

- Added ketone/alcohol/aldehyde parsing (GROUP_TYPE logic)

## Video explaining the whole project

https://drive.google.com/file/d/1C4P9izYF66jv2q7ZK_W_yZeXOqgW9UEQ/view?usp=drivesdk
