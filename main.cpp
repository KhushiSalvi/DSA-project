#include <bits/stdc++.h>
using namespace std;


int main()
{
    // Assuming Graph is defined elsewhere:
    // Graph g;

    int choice;
    cin >> choice;

    if (choice == 1) {
        string name, type;
        double la = 0, lo = 0;
        cout << "Node name: "; cin >> name;
        cout << "Node type (hospital/police/hotel/none): "; 
        cin >> type;
        if (type == "none") type = "";
        cout << "Optional lat lon (or 0 0): ";
         cin >> la >> lo;
        // g.addNode(name, type, la, lo);

    } else if (choice == 2) {
        string u, v;
        double d, c, t;
        cout << "Edge (u v distance cost time): ";
        cin >> u >> v >> d >> c >> t;
        // g.addEdge(u, v, d, c, t);

    } else if (choice == 3) {
        string s, e, crit;
        double penalty = 5.0;
        cout << "Start: "; cin >> s;
        cout << "End: "; cin >> e;
        cout << "Criteria (distance/cost/time): "; cin >> crit;
        cout << "Traffic penalty (default 5): "; cin >> penalty;

        pair<double, vector<string>> res; 
        // res = g.shortestPath(s, e, crit, penalty);

        if (res.second.empty()) {
            cout << "No path found.\n";
        } else {
            cout << "Shortest path (" << crit << "): ";
            // printPath(res.second);
            cout << "Total " << crit << " = " << res.first << "\n";
            // history.pushAction({res.second, crit, res.first});
        }

    } else if (choice == 4) {
        string s, e, crit;
        double penalty = 5.0;
        cout << "Start: "; cin >> s;
        cout << "End: "; cin >> e;
        cout << "Criteria (distance/cost/time): "; cin >> crit;
        cout << "Traffic penalty (default 5): "; cin >> penalty;

        pair<double, vector<string>> res; // ✅ FIXED
        // res = g.aStar(s, e, crit, penalty);

        if (res.second.empty()) cout << "No path found.\n";
        else {
            cout << "A* path: ";
            // printPath(res.second);
            cout << "Total " << crit << " = " << res.first << "\n";
            // history.pushAction({res.second, crit, res.first});
        }

    } else if (choice == 5) {
        string s, e;
        double wd, wc, wt;
        cout << "Start: "; cin >> s;
        cout << "End: "; cin >> e;
        cout << "Enter weights (distance cost time): ";
        cin >> wd >> wc >> wt;

        pair<double, vector<string>> res; // ✅ FIXED
        // res = g.multiCriteriaRoute(s, e, wd, wc, wt);

        if (res.second.empty()) cout << "No path found.\n";
        else {
            cout << "Multi-criteria path:\n";
            // printPath(res.second);
            cout << "Total combined = " << res.first << "\n";
            // history.pushAction({res.second, "multi", res.first});
        }

    } else if (choice == 6) {
        string s;
        cout << "Start node: "; cin >> s;

        vector<string> order;
        // order = g.bfs(s);

        if (order.empty()) cout << "Invalid start node.\n";
        else {
            cout << "BFS traversal order:\n";
            for (auto &n : order)
             cout << n << " ";
            cout << "\n";
        }
    }

    return 0;
   }