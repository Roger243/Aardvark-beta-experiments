#include "logistic_regression.hpp"

#include <algorithm>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <limits>
#include <numeric>
#include <random>
#include <sstream>
#include <stdexcept>

namespace winagent {
namespace {

constexpr double kEpsilon = 1.0e-15;

std::vector<std::string> split_csv_line(const std::string& line) {
    std::vector<std::string> cells;
    std::string cell;
    std::stringstream stream(line);

    while (std::getline(stream, cell, ',')) {
        cells.push_back(cell);
    }

    if (!line.empty() && line.back() == ',') {
        cells.emplace_back();
    }

    return cells;
}

std::vector<double> parse_numeric_row(const std::vector<std::string>& cells, int skip_column) {
    std::vector<double> row;
    row.reserve(cells.size());

    for (std::size_t index = 0; index < cells.size(); ++index) {
        if (static_cast<int>(index) == skip_column) {
            continue;
        }

        row.push_back(std::stod(cells[index]));
    }

    return row;
}

void validate_features(const std::vector<std::vector<double>>& x) {
    if (x.empty()) {
        throw std::invalid_argument("dataset is empty");
    }

    const std::size_t width = x.front().size();
    if (width == 0) {
        throw std::invalid_argument("dataset has no feature columns");
    }

    for (const auto& row : x) {
        if (row.size() != width) {
            throw std::invalid_argument("dataset rows have inconsistent widths");
        }
    }
}

}  // namespace

LogisticRegression::LogisticRegression(double eta, std::size_t n_iter, std::uint32_t random_state)
    : eta_(eta), n_iter_(n_iter), random_state_(random_state) {
    if (!(eta > 0.0 && eta <= 1.0)) {
        throw std::invalid_argument("eta must be in the range (0.0, 1.0]");
    }

    if (n_iter == 0) {
        throw std::invalid_argument("n_iter must be greater than zero");
    }
}

void LogisticRegression::fit(const std::vector<std::vector<double>>& x, const std::vector<int>& y) {
    validate_features(x);

    if (x.size() != y.size()) {
        throw std::invalid_argument("feature and label counts differ");
    }

    for (const int label : y) {
        if (label != 0 && label != 1) {
            throw std::invalid_argument("labels must be 0 or 1");
        }
    }

    std::mt19937 generator(random_state_);
    std::normal_distribution<double> distribution(0.0, 0.01);

    weights_.assign(x.front().size() + 1, 0.0);
    for (double& weight : weights_) {
        weight = distribution(generator);
    }

    costs_.clear();
    costs_.reserve(n_iter_);

    for (std::size_t epoch = 0; epoch < n_iter_; ++epoch) {
        std::vector<double> errors(x.size(), 0.0);
        double cost = 0.0;

        for (std::size_t row_index = 0; row_index < x.size(); ++row_index) {
            const double output = sigmoid(net_input(x[row_index]));
            const double clipped = std::clamp(output, kEpsilon, 1.0 - kEpsilon);
            errors[row_index] = static_cast<double>(y[row_index]) - output;
            cost += -static_cast<double>(y[row_index]) * std::log(clipped) -
                    (1.0 - static_cast<double>(y[row_index])) * std::log(1.0 - clipped);
        }

        for (std::size_t feature = 0; feature < x.front().size(); ++feature) {
            double gradient = 0.0;
            for (std::size_t row_index = 0; row_index < x.size(); ++row_index) {
                gradient += x[row_index][feature] * errors[row_index];
            }
            weights_[feature + 1] += eta_ * gradient;
        }

        weights_[0] += eta_ * std::accumulate(errors.begin(), errors.end(), 0.0);
        costs_.push_back(cost);
    }
}

std::vector<int> LogisticRegression::predict(const std::vector<std::vector<double>>& x) const {
    std::vector<int> labels;
    labels.reserve(x.size());

    for (const double probability : predict_proba(x)) {
        labels.push_back(probability >= 0.5 ? 1 : 0);
    }

    return labels;
}

std::vector<double> LogisticRegression::predict_proba(const std::vector<std::vector<double>>& x) const {
    if (weights_.empty()) {
        throw std::logic_error("model is not fitted");
    }

    validate_features(x);

    if (x.front().size() + 1 != weights_.size()) {
        throw std::invalid_argument("feature width does not match model weights");
    }

    std::vector<double> probabilities;
    probabilities.reserve(x.size());

    for (const auto& row : x) {
        probabilities.push_back(sigmoid(net_input(row)));
    }

    return probabilities;
}

const std::vector<double>& LogisticRegression::weights() const noexcept {
    return weights_;
}

const std::vector<double>& LogisticRegression::costs() const noexcept {
    return costs_;
}

void LogisticRegression::save(const std::string& path) const {
    if (weights_.empty()) {
        throw std::logic_error("cannot save an unfitted model");
    }

    std::ofstream output(path);
    if (!output) {
        throw std::runtime_error("failed to open model for writing: " + path);
    }

    output << std::setprecision(17) << eta_ << ' ' << n_iter_ << ' ' << random_state_ << '\n';
    output << weights_.size() << '\n';
    for (const double weight : weights_) {
        output << std::setprecision(17) << weight << '\n';
    }
}

LogisticRegression LogisticRegression::load(const std::string& path) {
    std::ifstream input(path);
    if (!input) {
        throw std::runtime_error("failed to open model for reading: " + path);
    }

    double eta = 0.05;
    std::size_t n_iter = 100;
    std::uint32_t random_state = 1;
    std::size_t count = 0;

    input >> eta >> n_iter >> random_state;
    input >> count;

    LogisticRegression model(eta, n_iter, random_state);
    model.weights_.resize(count);

    for (double& weight : model.weights_) {
        input >> weight;
    }

    if (!input || model.weights_.empty()) {
        throw std::runtime_error("invalid model file: " + path);
    }

    return model;
}

double LogisticRegression::sigmoid(double z) noexcept {
    const double clipped = std::clamp(z, -250.0, 250.0);
    return 1.0 / (1.0 + std::exp(-clipped));
}

double LogisticRegression::cost_for_one(double z) noexcept {
    return -std::log(std::clamp(sigmoid(z), kEpsilon, 1.0));
}

double LogisticRegression::cost_for_zero(double z) noexcept {
    return -std::log(std::clamp(1.0 - sigmoid(z), kEpsilon, 1.0));
}

double LogisticRegression::net_input(const std::vector<double>& row) const {
    double result = weights_[0];
    for (std::size_t index = 0; index < row.size(); ++index) {
        result += row[index] * weights_[index + 1];
    }
    return result;
}

Dataset read_training_csv(const std::string& path, bool has_header, int label_column) {
    std::ifstream input(path);
    if (!input) {
        throw std::runtime_error("failed to open training CSV: " + path);
    }

    Dataset dataset;
    std::string line;

    if (has_header) {
        std::getline(input, line);
    }

    while (std::getline(input, line)) {
        if (line.empty()) {
            continue;
        }

        const std::vector<std::string> cells = split_csv_line(line);
        if (cells.size() < 2) {
            throw std::invalid_argument("training CSV requires at least one feature and one label");
        }

        int resolved_label_column = label_column;
        if (resolved_label_column < 0) {
            resolved_label_column = static_cast<int>(cells.size()) - 1;
        }

        if (resolved_label_column < 0 || resolved_label_column >= static_cast<int>(cells.size())) {
            throw std::invalid_argument("label column is out of range");
        }

        dataset.y.push_back(std::stoi(cells[resolved_label_column]));
        dataset.x.push_back(parse_numeric_row(cells, resolved_label_column));
    }

    validate_features(dataset.x);
    return dataset;
}

std::vector<std::vector<double>> read_feature_csv(const std::string& path, bool has_header) {
    std::ifstream input(path);
    if (!input) {
        throw std::runtime_error("failed to open feature CSV: " + path);
    }

    std::vector<std::vector<double>> rows;
    std::string line;

    if (has_header) {
        std::getline(input, line);
    }

    while (std::getline(input, line)) {
        if (line.empty()) {
            continue;
        }
        rows.push_back(parse_numeric_row(split_csv_line(line), -1));
    }

    validate_features(rows);
    return rows;
}

void write_predictions_csv(const std::string& path, const std::vector<int>& labels, const std::vector<double>& probabilities) {
    if (labels.size() != probabilities.size()) {
        throw std::invalid_argument("prediction labels and probabilities differ in length");
    }

    std::ofstream output(path);
    if (!output) {
        throw std::runtime_error("failed to open predictions CSV for writing: " + path);
    }

    output << "label,probability\n";
    for (std::size_t index = 0; index < labels.size(); ++index) {
        output << labels[index] << ',' << std::setprecision(17) << probabilities[index] << '\n';
    }
}

void write_cost_curve_csv(const std::string& path, double start, double stop, double step) {
    if (!(step > 0.0) || !(stop > start)) {
        throw std::invalid_argument("cost curve requires start < stop and step > 0");
    }

    std::ofstream output(path);
    if (!output) {
        throw std::runtime_error("failed to open cost curve CSV for writing: " + path);
    }

    output << "z,phi_z,cost_y_1,cost_y_0\n";
    for (double z = start; z < stop; z += step) {
        const double phi = LogisticRegression::sigmoid(z);
        output << std::setprecision(17) << z << ',' << phi << ','
               << LogisticRegression::cost_for_one(z) << ','
               << LogisticRegression::cost_for_zero(z) << '\n';
    }
}

}  // namespace winagent
