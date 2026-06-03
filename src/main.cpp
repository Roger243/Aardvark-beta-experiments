#include "logistic_regression.hpp"

#include <exception>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

namespace {

std::unordered_map<std::string, std::string> parse_args(int argc, char** argv) {
    std::unordered_map<std::string, std::string> args;

    for (int index = 1; index < argc; ++index) {
        std::string key = argv[index];
        if (key.rfind("--", 0) != 0) {
            throw std::invalid_argument("unexpected argument: " + key);
        }

        if (index + 1 >= argc || std::string(argv[index + 1]).rfind("--", 0) == 0) {
            args[key] = "true";
        } else {
            args[key] = argv[++index];
        }
    }

    return args;
}

std::string get_required(const std::unordered_map<std::string, std::string>& args, const std::string& key) {
    const auto iter = args.find(key);
    if (iter == args.end()) {
        throw std::invalid_argument("missing required argument: " + key);
    }
    return iter->second;
}

std::string get_or_default(const std::unordered_map<std::string, std::string>& args, const std::string& key, const std::string& fallback) {
    const auto iter = args.find(key);
    return iter == args.end() ? fallback : iter->second;
}

bool has_flag(const std::unordered_map<std::string, std::string>& args, const std::string& key) {
    return args.find(key) != args.end();
}

void print_usage() {
    std::cout
        << "winagent-ai Windows 11 local AI agent\n"
        << "\n"
        << "Commands:\n"
        << "  --train train.csv --model model.txt [--eta 0.05] [--n-iter 100] [--header]\n"
        << "  --predict features.csv --model model.txt --output predictions.csv [--header]\n"
        << "  --agent-once train.csv --predict features.csv --model model.txt --output predictions.csv [--header]\n"
        << "  --cost-curve costs.csv\n";
}

}  // namespace

int main(int argc, char** argv) {
    try {
        const auto args = parse_args(argc, argv);

        if (argc == 1 || has_flag(args, "--help")) {
            print_usage();
            return 0;
        }

        if (has_flag(args, "--cost-curve")) {
            winagent::write_cost_curve_csv(get_required(args, "--cost-curve"), -10.0, 10.0, 0.1);
            return 0;
        }

        const bool has_header = has_flag(args, "--header");

        if (has_flag(args, "--agent-once")) {
            const auto training = winagent::read_training_csv(get_required(args, "--agent-once"), has_header, -1);
            winagent::LogisticRegression model(
                std::stod(get_or_default(args, "--eta", "0.05")),
                static_cast<std::size_t>(std::stoul(get_or_default(args, "--n-iter", "100"))),
                static_cast<std::uint32_t>(std::stoul(get_or_default(args, "--random-state", "1"))));

            model.fit(training.x, training.y);
            model.save(get_required(args, "--model"));

            const auto features = winagent::read_feature_csv(get_required(args, "--predict"), has_header);
            const auto probabilities = model.predict_proba(features);
            const auto labels = model.predict(features);
            winagent::write_predictions_csv(get_required(args, "--output"), labels, probabilities);
            return 0;
        }

        if (has_flag(args, "--train")) {
            const auto training = winagent::read_training_csv(get_required(args, "--train"), has_header, -1);
            winagent::LogisticRegression model(
                std::stod(get_or_default(args, "--eta", "0.05")),
                static_cast<std::size_t>(std::stoul(get_or_default(args, "--n-iter", "100"))),
                static_cast<std::uint32_t>(std::stoul(get_or_default(args, "--random-state", "1"))));

            model.fit(training.x, training.y);
            model.save(get_required(args, "--model"));

            std::cout << "trained epochs=" << model.costs().size()
                      << " final_cost=" << model.costs().back() << '\n';
            return 0;
        }

        if (has_flag(args, "--predict")) {
            const auto model = winagent::LogisticRegression::load(get_required(args, "--model"));
            const auto features = winagent::read_feature_csv(get_required(args, "--predict"), has_header);
            const auto probabilities = model.predict_proba(features);
            const auto labels = model.predict(features);
            winagent::write_predictions_csv(get_required(args, "--output"), labels, probabilities);
            return 0;
        }

        print_usage();
        return 1;
    } catch (const std::exception& error) {
        std::cerr << "error: " << error.what() << '\n';
        return 2;
    }
}
