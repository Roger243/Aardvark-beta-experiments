#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace winagent {

class LogisticRegression {
public:
    LogisticRegression(double eta = 0.05, std::size_t n_iter = 100, std::uint32_t random_state = 1);

    void fit(const std::vector<std::vector<double>>& x, const std::vector<int>& y);
    std::vector<int> predict(const std::vector<std::vector<double>>& x) const;
    std::vector<double> predict_proba(const std::vector<std::vector<double>>& x) const;

    const std::vector<double>& weights() const noexcept;
    const std::vector<double>& costs() const noexcept;

    void save(const std::string& path) const;
    static LogisticRegression load(const std::string& path);

    static double sigmoid(double z) noexcept;
    static double cost_for_one(double z) noexcept;
    static double cost_for_zero(double z) noexcept;

private:
    double net_input(const std::vector<double>& row) const;
    double eta_;
    std::size_t n_iter_;
    std::uint32_t random_state_;
    std::vector<double> weights_;
    std::vector<double> costs_;
};

struct Dataset {
    std::vector<std::vector<double>> x;
    std::vector<int> y;
};

Dataset read_training_csv(const std::string& path, bool has_header, int label_column);
std::vector<std::vector<double>> read_feature_csv(const std::string& path, bool has_header);
void write_predictions_csv(const std::string& path, const std::vector<int>& labels, const std::vector<double>& probabilities);
void write_cost_curve_csv(const std::string& path, double start, double stop, double step);

}  // namespace winagent
