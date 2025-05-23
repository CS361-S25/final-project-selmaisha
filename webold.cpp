#define UIT_VENDORIZE_EMP
#define UIT_SUPPRESS_MACRO_INSEEP_WARNINGS

// Include necessary libraries for web interface, random number generation, and world simulation
#include "emp/math/Random.hpp"
#include "emp/prefab/Card.hpp"
#include "emp/prefab/ReadoutPanel.hpp"
#include "emp/web/Animate.hpp"
#include "emp/web/Document.hpp"
#include "emp/web/web.hpp"
#include "World.h"
#include "Org.h"
#include "emp/config/ArgManager.hpp"
#include "emp/prefab/ConfigPanel.hpp"
#include "emp/web/UrlParams.hpp"
#include "ConfigSetup.h"

// Declare global document objects for web interface
emp::web::Document doc("target");
emp::web::Document settings("settings");
emp::web::Document buttons("buttons");
emp::web::Document panel("panel");
emp::web::Document explanation("explanation");

// Global config for the world setup
WorldConfig config;

/**
 * input: none
 * output: none
 * purpose: Handles the animation and display of the simulation, including grid drawing, organism movement, and updates to the world.
 */
class AEAnimator : public emp::web::Animate {

    // Define arena dimensions and grid cell size
    const int num_h_boxes = 15; // height of the grid (to be configurable)
    const int num_w_boxes = 15; // width of the grid (to be configurable)
    const double RECT_SIDE = 15; // size of each grid cell
    const double width{num_w_boxes * RECT_SIDE};
    const double height{num_h_boxes * RECT_SIDE};
    int update_count = 0;
    emp::Random random{config.SEED()}; // Initialize random number generator with seed
    OrgWorld world{random, &config};   // Create the world simulation

    // Initialize the canvas for drawing the world
    emp::web::Canvas canvas{width, height, "canvas"};

    public:

        /**
         * input: none
         * output: none (side-effect: initializes simulation, config panels, and web interface elements)
         * purpose: Initializes the AEAnimator, sets up world grid, injects organisms, sets up configuration panel and web interface.
         */
        AEAnimator() {
            SetupCanvasAndControls();
            ApplyConfigFromArgs();
            SetupConfigPanel();
            DisplayExplanation();
            InitializeWorld();
            SetupReadoutPanel();
        }

        /**
     * input: none
     * output: none (side-effect: updates the world state and redraws the grid)
     * purpose: Updates the world for the current frame, redrawing the grid with new state (tasks completed, etc.)
     */
    void DoFrame() override {
        canvas.Clear();
        world.Update();
        update_count++;

        // Iterate over the grid and draw each cell, coloring based on task completion
        int org_num = 0;
        for (int x = 0; x < num_w_boxes; x++) {
            for (int y = 0; y < num_h_boxes; y++) {
                if (world.IsOccupied(org_num)) {
                    // Change color based on the task completed by the organism
                    if (world.GetOrg(org_num).GetCPU().state.completed_square) {
                        canvas.Rect(x * RECT_SIDE, y * RECT_SIDE, RECT_SIDE, RECT_SIDE, "blue", "black");
                    } else if (world.GetOrg(org_num).GetCPU().state.completed_add) {
                        canvas.Rect(x * RECT_SIDE, y * RECT_SIDE, RECT_SIDE, RECT_SIDE, "red", "black");
                    } else if (world.GetOrg(org_num).GetCPU().state.completed_addthree) {
                        canvas.Rect(x * RECT_SIDE, y * RECT_SIDE, RECT_SIDE, RECT_SIDE, "purple", "black");
                    } else if (world.GetOrg(org_num).GetCPU().state.completed_subtract) {
                        canvas.Rect(x * RECT_SIDE, y * RECT_SIDE, RECT_SIDE, RECT_SIDE, "orange", "black");
                    } else if (world.GetOrg(org_num).GetCPU().state.completed_double) {
                        canvas.Rect(x * RECT_SIDE, y * RECT_SIDE, RECT_SIDE, RECT_SIDE, "yellow", "black");
                    } else if (world.GetOrg(org_num).GetCPU().state.completed_decrement) {
                        canvas.Rect(x * RECT_SIDE, y * RECT_SIDE, RECT_SIDE, RECT_SIDE, "green", "black");
                    } else if (world.GetOrg(org_num).GetCPU().state.completed_increment) {
                        canvas.Rect(x * RECT_SIDE, y * RECT_SIDE, RECT_SIDE, RECT_SIDE, "pink", "black");
                    } else if (world.GetOrg(org_num).GetCPU().state.completed_echo) {
                        canvas.Rect(x * RECT_SIDE, y * RECT_SIDE, RECT_SIDE, RECT_SIDE, "cyan", "black");
                    } else if (world.GetOrg(org_num).GetCPU().state.completed_bitnot) {
                        canvas.Rect(x * RECT_SIDE, y * RECT_SIDE, RECT_SIDE, RECT_SIDE, "brown", "black");
                    } else {
                        canvas.Rect(x * RECT_SIDE, y * RECT_SIDE, RECT_SIDE, RECT_SIDE, "black", "black");
                    }
                } else {
                    canvas.Rect(x * RECT_SIDE, y * RECT_SIDE, RECT_SIDE, RECT_SIDE, "white", "black");
                }
                org_num++;
            }
        }
    }

    private:
        void SetupCanvasAndControls() {
            doc << canvas;
            buttons << GetToggleButton("Toggle");
            buttons << GetStepButton("Step");
        }

        void ApplyConfigFromArgs() {
            auto specs = emp::ArgManager::make_builtin_specs(&config);
            emp::ArgManager am(emp::web::GetUrlParams(), specs);
            am.UseCallbacks();
            if (am.HasUnused()) std::exit(EXIT_FAILURE);
        }

        void SetupConfigPanel() {
            emp::prefab::ConfigPanel config_panel(config);
            config_panel.ExcludeSetting("SEED");
            config_panel.ExcludeSetting("NUM_UPDATES");
            config_panel.ExcludeSetting("NUM_BOXES");
            config_panel.SetRange("NUM_START", "1", "100", "1");
            config_panel.SetRange("MUTATION_RATE", "0", "0.1");
            settings << config_panel;
        }

        void DisplayExplanation() {
            explanation << "This is a test of the Empirical framework.<br> "
                        << "It is a simple grid world with organisms that can move around and "
                        << "complete tasks. The organisms are initialized with random genomes, "
                        << "and they can mutate their genomes as they reproduce. <br>"
                        << "The goal is to see how quickly the organisms can evolve tasks.<br>";
            explanation << "The tasks are: square, double, add, add three, subtract, decrement, increment, echo, and bitnot. <br>";
            explanation << "The colors: blue = square, red = add, purple = add three, orange = subtract, yellow = double, green = decrement, pink = increment, cyan = echo, brown = bitnot. <br>"
                        << "The black squares don't solve tasks, and the white squares are empty. <br>";
            explanation << "If an organism solves multiple tasks, only the first color (from the above list) will be shown."
                        << "The counters above will still count all organisms though.<br>";
        }

        void InitializeWorld() {
            random.ResetSeed(config.SEED());
            Organism* new_org;
            world.SetPopStruct_Grid(num_w_boxes, num_h_boxes);
            for (int i = 0; i < config.NUM_START(); i++) {
                new_org = new Organism(&world, 0);
                world.Inject(*new_org);
            }
            world.Resize(num_h_boxes, num_w_boxes);
        }

        void SetupReadoutPanel() {
            emp::prefab::ReadoutPanel values("Readout Values", 100);
            values.AddValues(
                "Organisms", "How many organisms are present", [this]() {
                    return emp::to_string(world.GetNumOrgs());
                },
                "Echo solvers", "How many organisms are solving echo tasks", [this]() {
                    return emp::to_string(world.GetAllTaskSolvers().echo_solvers);
                },
                "Increment solvers", "How many organisms are solving add tasks", [this]() {
                    return emp::to_string(world.GetAllTaskSolvers().increment_solvers);
                },
                "Decrement solvers", "How many organisms are solving subtract tasks", [this]() {
                    return emp::to_string(world.GetAllTaskSolvers().decrement_solvers);
                },
                "Add solvers", "How many organisms are solving add tasks", [this]() {
                    return emp::to_string(world.GetAllTaskSolvers().add_solvers);
                },
                "Square solvers", "How many organisms are solving square tasks", [this]() {
                    return emp::to_string(world.GetAllTaskSolvers().square_solvers);
                },
                "Double solvers", "How many organisms are solving double tasks", [this]() {
                    return emp::to_string(world.GetAllTaskSolvers().double_solvers);
                },
                "Add three solvers", "How many organisms are solving add three tasks", [this]() {
                    return emp::to_string(world.GetAllTaskSolvers().addthree_solvers);
                },
                "Subtract solvers", "How many organisms are solving subtract tasks", [this]() {
                    return emp::to_string(world.GetAllTaskSolvers().subtract_solvers);
                },
                "Bitnot solvers", "How many organisms are solving bitnot tasks", [this]() {
                    return emp::to_string(world.GetAllTaskSolvers().bitnot_solvers);
                },
                "Update step", "Current update step", [this]() {
                    return emp::to_string(update_count);
                }
            );
            panel << values;
        }

};

// Instantiate the animator object to start the simulation
AEAnimator animator;

/**
 * input: none
 * output: none
 * purpose: Starts the simulation by triggering the animator step.
 */
int main() { animator.Step(); }
