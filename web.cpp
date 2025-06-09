
#define UIT_VENDORIZE_EMP
#define UIT_SUPPRESS_MACRO_INSEEP_WARNINGS

#include "emp/web/web.hpp"
#include "emp/web/Animate.hpp"
#include "emp/web/Canvas.hpp"
#include "emp/prefab/ConfigPanel.hpp"
#include "emp/web/init.hpp"
#include "emp/web/UrlParams.hpp"
#include "emp/prefab/ReadoutPanel.hpp"
#include "emp/math/Random.hpp"



#include "ConfigSetup.h"
#include "World.h"
#include "Parasite.h"

WorldConfig config;

emp::web::Document doc("target");
emp::web::Document settings("settings");
emp::web::Document panel("panel");

emp::web::Document explanation_doc("explanation");
emp::web::Document paper_results_doc("paper_results");

/**
 * Constructor
 * Input: none
 * Output: none
 * Purpose: Initialize the animation, world, UI controls, and configuration panel
 */
class BaselineAnimator : public emp::web::Animate {

  int num_boxes = 32;
  double canvas_max_px = 600.0;
  double RECT_SIDE = canvas_max_px / 15;

  double width = canvas_max_px;
  double height = canvas_max_px;

  int update_count = 0;

  bool parasites_injected = false;

  emp::Ptr<emp::Random> random;
  emp::Ptr<OrgWorld> world;

  emp::web::Canvas canvas{width, height, "canvas"};
  emp::prefab::ConfigPanel config_panel;

public:
  BaselineAnimator() 
  : config_panel(config) {
    ApplyConfigFromArgs();  // Load from URL if available
    random.New(config.SEED());
    world.New(*random, &config);

    SetupCanvasAndControls();
    SetupConfigPanel();
    WriteExplanation();
    WriteExplanationResults();
    InitializeWorld();
    SetupReadoutPanel();
    WriteRecommendedSettings();
  }

  /**
  * DoFrame
  * Input: none
  * Output: none
  * Purpose: Main update loop for each animation frame. Injects parasites and updates world state.
  */
  void DoFrame() override {
    if (update_count >= config.NUM_UPDATES()) {
      this->Stop();
    }
    if (update_count == config.INJECT_PARASITES_AT() && !parasites_injected) {
      std::cout << "Injecting parasites at update " << update_count << std::endl;

        for (int i = 0; i < config.NUM_PARASITES(); i++) {
          auto* parasite = new Parasite(world, -1.0);
          parasite->setVirulence(config.VIRULENCE());
          world->InjectParasite(parasite);
        }

        parasites_injected = true;
      }
      world->Update();
      update_count++;
      Draw();
  }

private:

/**
  * SetupCanvasAndControls
  * Input: none
  * Output: none
  * Purpose: Add canvas and interactive buttons (toggle, step, apply settings) to the document
  */
  void SetupCanvasAndControls() {
    doc << canvas;

    doc << GetToggleButton("Toggle");
    doc << GetStepButton("Step");


    doc << emp::web::Button([this]() {
      ResetWorldFromConfig();
    }, "Apply Settings");
  }


 /**
 * ApplyConfigFromArgs
 * Input: none
 * Output: none
 * Purpose: Load configuration values from URL parameters
 */
  void ApplyConfigFromArgs() {
    auto specs = emp::ArgManager::make_builtin_specs(&config);
    emp::ArgManager am(emp::web::GetUrlParams(), specs);
    am.UseCallbacks();  // Updates config from URL
  }


  /**
  * SetupConfigPanel
  * Input: none
  * Output: none
  * Purpose: Set up the GUI panel for modifying simulation parameters
  */
  void SetupConfigPanel() {
    emp::prefab::ConfigPanel config_panel(config);
    config_panel.ExcludeSetting("SEED");
    config_panel.ExcludeSetting("OUTPUT_DIR");
    config_panel.ExcludeSetting("RUN_TIME");

    config_panel.SetRange("NUM_START", "1", "100", "1");
    config_panel.SetRange("MUTATION_RATE", "0", "0.1");
    config_panel.SetRange("LIFE_SPAN", "1", "1000", "1");
    config_panel.SetRange("VIRULENCE", "0", "5", "0.1");
    config_panel.SetRange("INJECT_PARASITES_AT", "0", "20000", "500");
    config_panel.SetRange("NUM_PARASITES", "0", "1000", "1");
    config_panel.SetRange("BONUS_UPDATE_LIMIT", "0", "20000", "500");
    config_panel.SetRange("NUM_UPDATES", "1", "100000", "1000");
    config_panel.SetRange("NUM_BOXES", "1", "100", "1");

    settings << config_panel;
  }

  /**
  * InitializeWorld
  * Input: none
  * Output: none
  * Purpose: Configure the world structure, resize the grid, and populate with initial hosts
  */
  void InitializeWorld() {

    // world->SetPopStruct_Mixed();
    // world->Resize(num_h_boxes, num_w_boxes);
    num_boxes = config.NUM_BOXES();
    RECT_SIDE = canvas_max_px / num_boxes;
    width = height = canvas_max_px;
    canvas.SetWidth(width);
    canvas.SetHeight(height);

    world->Resize(num_boxes, num_boxes);
    for (int i = 0; i < config.NUM_START(); ++i) {
      size_t pos = world->GetRandom().GetUInt(world->GetSize());
      Host* host = new Host(world, 0);
      world->InjectAt(*host, pos);
    }
    
    update_count = 0;
    Draw();
  }

  /**
  * ResetWorldFromConfig
  * Input: none
  * Output: none
  * Purpose: Reset the simulation using updated config parameters and reseed RNG
  */
  void ResetWorldFromConfig() {
    ApplyConfigFromArgs();                  // Reload config from URL panel
    random->ResetSeed(config.SEED());       // Reseed RNG
    world.Delete();                         // Clean up the old world
    world.New(*random, &config);            // Allocate a new world with new config
    InitializeWorld();                      // Re-inject organisms, resize world
    
  }

  /**
  * Draw
  * Input: none
  * Output: none
  * Purpose: Visualize current state of the grid by drawing organisms and parasites on canvas
  */
  void Draw() {
    canvas.Clear();
    const auto& pop = world->GetPopulation();
    size_t org_num = 0;

    for (int x = 0; x < num_boxes; ++x) {
      for (int y = 0; y < num_boxes; ++y) {
        if (world->IsOccupied(org_num)) {
          const auto& org_ptr = pop[org_num];
          if (org_ptr) {
            std::string color = org_ptr->GetTaskColor();  
            canvas.Rect(x * RECT_SIDE, y * RECT_SIDE, RECT_SIDE - 1, RECT_SIDE - 1, color, "black");
          }
          if (org_ptr->HasParasite()) {
            auto parasite = org_ptr->GetParasite();
            // Draw the parasite's task color
            std::string color = parasite->GetTaskColor();
            canvas.Circle(x * RECT_SIDE + RECT_SIDE / 2,
                          y * RECT_SIDE + RECT_SIDE / 2,
                          RECT_SIDE / 4, color, "red");
          }
        } else {
          canvas.Rect(x * RECT_SIDE, y * RECT_SIDE, RECT_SIDE - 1, RECT_SIDE - 1, "white", "black");
        }
        ++org_num;
      }
    }
  }

  /**
  * SetupReadoutPanel
  * Input: none
  * Output: none
  * Purpose: Create and attach a panel that displays simulation statistics and task counts
  */
  void SetupReadoutPanel() {
    emp::prefab::ReadoutPanel values("Readout Values", 100);
    values.AddValues(
      "Organisms", "How many organisms are present", [this]() {
        return emp::to_string(world->GetNumOrgs());
      },
      "NOT solvers", "How many organisms are solving NOT", [this]() {
        return emp::to_string(world->GetAllTaskSolvers().NOT_count);
      },
      "NAND solvers", "How many organisms are solving NAND", [this]() {
        return emp::to_string(world->GetAllTaskSolvers().NAND_count);
      },
      "AND solvers", "How many organisms are solving AND", [this]() {
        return emp::to_string(world->GetAllTaskSolvers().AND_count);
      },
      "ORN solvers", "How many organisms are solving ORN", [this]() {
        return emp::to_string(world->GetAllTaskSolvers().ORN_count);
      },
      "OR solvers", "How many organisms are solving OR", [this]() {
        return emp::to_string(world->GetAllTaskSolvers().OR_count);
      },
      "ANDN solvers", "How many organisms are solving ANDN", [this]() {
        return emp::to_string(world->GetAllTaskSolvers().ANDN_count);
      },
      "NOR solvers", "How many organisms are solving NOR", [this]() {
        return emp::to_string(world->GetAllTaskSolvers().NOR_count);
      },
      "XOR solvers", "How many organisms are solving XOR", [this]() {
        return emp::to_string(world->GetAllTaskSolvers().XOR_count);
      },
      "EQU solvers", "How many organisms are solving EQU", [this]() {
        return emp::to_string(world->GetAllTaskSolvers().EQU_count);
      },
      "Update step", "Current update step", [this]() {
        return emp::to_string(update_count);
      },
      "Parasites", "How many parasites are present", [this]() {
        return emp::to_string(world->GetParasiteCountDataNode().GetCurrent());
      },

      // Parasite task counts
    "Parasite NOT", "Parasites solving NOT", [this]() {
      return emp::to_string(world->GetParasiteNOTCountDataNode().GetCurrent());
    },
    "Parasite NAND", "Parasites solving NAND", [this]() {
      return emp::to_string(world->GetParasiteNANDCountDataNode().GetCurrent());
    },
    "Parasite AND", "Parasites solving AND", [this]() {
      return emp::to_string(world->GetParasiteANDCountDataNode().GetCurrent());
    },
    "Parasite ORN", "Parasites solving ORN", [this]() {
      return emp::to_string(world->GetParasiteORNCountDataNode().GetCurrent());
    },
    "Parasite OR", "Parasites solving OR", [this]() {
      return emp::to_string(world->GetParasiteORCountDataNode().GetCurrent());
    },
    "Parasite ANDN", "Parasites solving ANDN", [this]() {
      return emp::to_string(world->GetParasiteANDNCountDataNode().GetCurrent());
    },
    "Parasite NOR", "Parasites solving NOR", [this]() {
      return emp::to_string(world->GetParasiteNORCountDataNode().GetCurrent());
    },
    "Parasite XOR", "Parasites solving XOR", [this]() {
      return emp::to_string(world->GetParasiteXORCountDataNode().GetCurrent());
    },
    "Parasite EQU", "Parasites solving EQU", [this]() {
      return emp::to_string(world->GetParasiteEQUCountDataNode().GetCurrent());
    }
    );
    panel << values;
  }

  /**
   * WriteExplanation
   * Input: none
   * Output: none Display the description of simulation rules in the explanation document
   */
  void WriteExplanation(){
    explanation_doc << R"(
    <div style='max-width:520px;margin:1em 0;padding:1em;border:1px solid #ccc;border-radius:8px;background:#f9f9f9;'>
      <h3>World Rules</h3>
      <ul>
        <li>
          The world contains a population of digital <b>hosts</b> and <b>parasites</b>. Each organism has a <b>CPU</b> that runs a program (genome) and can solve logic tasks (e.g., NOT, NAND, AND, etc.).
        </li>
        <li>
          <b>Hosts</b> and <b>parasites</b> earn <b>points</b> by solving tasks. Points affect their survival and ability to reproduce.
        </li>
        <li>
          <b>Parasites</b> live inside hosts and can only reproduce by infecting new hosts. Parasite reproduction is triggered when they solve the <b>same task</b> as their host.
        </li>
        <li>
          When a parasite solves the same task as its host, it earns bonus points and can reproduce. The <b>host loses points</b> proportional to the parasite's <b>virulence</b> (a parameter controlling how harmful the parasite is).
        </li>
        <li>
          <b>Virulence</b> determines how many points are transferred from the host to the parasite when a parasite successfully exploits its host by solving the same task.
        </li>
        <li>
          There is a <b>bonus period</b> (the "bonus time buffer") after parasites are first introduced. During this time, parasites receive extra points for solving tasks and do <b>not harm their hosts</b> (hosts do not lose points from parasite activity).
        </li>
        <li>
          Organisms and parasites <b>age</b> and die after reaching a lifespan or if their points drop too low.
        </li>
        <li>
          Reproduction is controlled by special instructions in the organism's program. Offspring inherit mutated copies of the parent's genome.
        </li>
        <li>
          Task-solving and population statistics are tracked and displayed in the readout.
        </li>
      </ul>

      <h3>Task Color Legend (Hosts & Parasites)</h3>
        <p>Each color below represents a logic task. Both hosts (squares cells) and parasites (red circles) are color-coded by the task they’ve solved.</p>
        <table style="border-collapse: collapse;">
          <tr><td style="color:blue;">&#9632;</td><td>NOT</td></tr>
          <tr><td style="color:red;">&#9632;</td><td>NAND</td></tr>
          <tr><td style="color:green;">&#9632;</td><td>AND</td></tr>
          <tr><td style="color:yellow;">&#9632;</td><td>ORN</td></tr>
          <tr><td style="color:purple;">&#9632;</td><td>OR</td></tr>
          <tr><td style="color:orange;">&#9632;</td><td>ANDN</td></tr>
          <tr><td style="color:pink;">&#9632;</td><td>NOR</td></tr>
          <tr><td style="color:cyan;">&#9632;</td><td>XOR</td></tr>
          <tr><td style="color:brown;">&#9632;</td><td>EQU</td></tr>
          <tr><td style="color:black;">&#9632;</td><td>None (has not yet solved any task)</td></tr>
        </table>

    </div>
    )";
  }

/**
 * WriteRecommendedSettings
 * Input: none
 * Output: none
 * Purpose: Display a styled info box in the explanation section that suggests recommended
 *          configuration values for running the simulation. These values are tuned to 
 *          demonstrate meaningful coevolutionary dynamics between hosts and parasites.
 */
  void WriteRecommendedSettings() {
    explanation_doc << R"(
      <div style='max-width:520px;margin:1em 0;padding:1em;border:1px solid #ddd;border-radius:8px;background:#f1faff;'>
        <h3>🔧 Recommended Simulation Settings</h3>
        <ul>
          <li><b>NUM_START = 10</b> — Start with 10 hosts.</li>
          <li><b>MUTATION_RATE = 0.02</b> — Host mutation rate.</li>
          <li><b>PARASITE_MUT_RATE = 0.04</b> — Parasite mutation rate.</li>
          <li><b>NUM_UPDATES = 4000</b> — Length of simulation.</li>
          <li><b>REWARDED = 20</b> — Points per task solved.</li>
          <li><b>NUM_BOXES = 20</b> — Grid size: 20x20.</li>
          <li><b>LIFE_SPAN = 30</b> — Organism lifespan.</li>
          <li><b>VIRULENCE = 0.8</b> — Parasite strength.</li>
          <li><b>NUM_PARASITES = 100</b> — Initial number of parasites.</li>
          <li><b>INJECT_PARASITES_AT = 1500</b> — Injection time.</li>
          <li><b>BONUS_UPDATE_LIMIT = 2000</b> — Parasite grace period.</li>
        </ul>
        <p style="font-style: italic; color: #333;">
          These values support stable host-parasite coevolution and task diversity.
        </p>
      </div>
    )";
  }

  /**
  * WriteExplanationResults
  * Input: none
  * Output: none
  * Purpose: Summarize findings from Zaman et al. (2011) in a styled results document
  */
  void WriteExplanationResults(){
    paper_results_doc << R"(
    <div style='max-width:520px;margin:1em 0;padding:1em;border:1px solid #ccc;border-radius:8px;background:#eef9f0;'>
      <h3>Key Results from Zaman et. al. (2011)</h3>
      <ul>
        <li>
          The study simulated coevolution between digital <b>hosts</b> and <b>parasites</b> in a grid-based environment, focusing on how <b>evolutionary dynamics</b> unfold when both species must solve logic tasks to reproduce.
        </li>
        <li>
          <b>Parasites evolve rapidly</b> to exploit common host strategies by copying their task-solving behaviors, which forces hosts to constantly evolve <b>new solutions</b> to avoid exploitation.
        </li>
        <li>
          This creates a continual arms race where hosts must "run just to stay in place" evolutionarily, leading to <b>ongoing innovation and diversity</b> in host genotypes.
        </li>
        <li>
          Systems with <b>coevolving parasites</b> showed more consistent and prolonged adaptation compared to systems without parasites, where host evolution plateaued more quickly.
        </li>
        <li>
          <b>Task diversity</b> in the population increases under coevolution. Hosts that can solve <b>unique or less common tasks</b> are harder for parasites to exploit and thus gain an advantage.
        </li>
        <li>
          The study also found that turning off mutations after 100,000 updates lead to less diversity than without turning mutations off, but still found more novel variation than when parasites were not present.
      </ul>
      <h4>Configuration Highlights</h4>
      <ul>
        <li>Platform: **Avida** digital evolution system with grid-based world.</li>
        <li>Tasks: Boolean logic functions (NOT, NAND, AND, OR, etc.).</li>
        <li>Parasite infection: occurs when parasite and host solve **at least one overlapping task** (“inverse gene‑for‑gene” model) :contentReference[oaicite:6]{index=6}.</li>
        <li>Reproduction: hosts and parasites reproduce asexually with mutation; parasites require infecting hosts (shared task) to replicate.</li>
        <li>Experimental conditions: runs with and without parasites; with mutations always on, and with mutations stopped at 100k updates and optionally resumed.</li>
        <li>Monitored over **200,000 updates**; diversity measured using **Shannon index**.</li>
      </ul>
    </div>
    )";
  }
};


BaselineAnimator animator;


int main() {
  emp::Initialize();
  animator.Step();  
}