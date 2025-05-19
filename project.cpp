#include <iostream>
#include <vector>
#include <iomanip>
#include <memory>  // For smart pointers

using namespace std;

// Base template class for ODE Solvers
template<typename T>
class ODESolver {
public:
    virtual vector<vector<T>> solve(T t, vector<T> y, T dt, int steps) = 0;
    virtual ~ODESolver() = default;
};

// Concrete RK4 Solver implementation (template)
template<typename T>
class RKSolver : public ODESolver<T> {
    vector<T> (*odeSystem)(T, vector<T>);  // ODE system function pointer

public:
    RKSolver(vector<T> (*system)(T, vector<T>)) : odeSystem(system) {}

    vector<vector<T>> solve(T t, vector<T> y, T dt, int steps) override {
        vector<vector<T>> results;
        results.push_back({t, y[0], y[1]});

        for (int i = 0; i < steps; i++) {
            vector<T> k1 = odeSystem(t, y);
            vector<T> k2 = odeSystem(t + dt / 2, {y[0] + dt * k1[0] / 2, y[1] + dt * k1[1] / 2});
            vector<T> k3 = odeSystem(t + dt / 2, {y[0] + dt * k2[0] / 2, y[1] + dt * k2[1] / 2});
            vector<T> k4 = odeSystem(t + dt, {y[0] + dt * k3[0], y[1] + dt * k3[1]});

            y[0] += dt * (k1[0] + 2 * k2[0] + 2 * k3[0] + k4[0]) / 6;
            y[1] += dt * (k1[1] + 2 * k2[1] + 2 * k3[1] + k4[1]) / 6;
            t += dt;

            results.push_back({t, y[0], y[1]});
        }
        return results;
    }
};

// Lotka-Volterra equations (template)
template<typename T>
vector<T> lotkaVolterra(T t, vector<T> populations) {
    const T a = 0.1, b = 0.02, d = 0.01, g = 0.1;
    T prey = populations[0], predator = populations[1];
    return {
        a * prey - b * prey * predator,  // dPrey/dt
        d * prey * predator - g * predator  // dPredator/dt
    };
}

int main() {
    double prey, predator;
    cout << "Enter initial prey and predator densities: ";
    cin >> prey >> predator;

    // Create a unique_ptr to manage the RKSolver dynamically
    unique_ptr<RKSolver<double>> solver = make_unique<RKSolver<double>>(lotkaVolterra<double>);

    // Run simulation
    vector<vector<double>> results = solver->solve(0.0, {prey, predator}, 0.1, 100);

    // Output results
    cout << "Time\tPrey\tPredator\n";
    cout << fixed << setprecision(2);
    for (const auto& row : results) {
        cout << row[0] << "\t" << row[1] << "\t" << row[2] << '\n';
    }

    return 0;
}
