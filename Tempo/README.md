###Tempo - Robust and Self-Tuning Resource Management in Multi-tenant Parallel Databases

Popular cluster resource managers (RMs) today do not provide direct support for performance objectives such as: “Average job response time of tenant A must be less than two minutes”, or “No more than 5% of tenant B’s jobs can miss the deadline of 1 hour.” Thus, cluster operators have to tinker with the RM’s low-level configuration settings to meet such objectives.

Tempo is a framework that brings several desirable properties to existing RMs:
* *Simplicity.* Tempo a simple interface for DBAs to specify performance objectives (SLOs) declaratively.
* *Self-tuning.* Tempo adapts to workload patterns and variations.
* *Robustness.*
  * Makes high-quality resource scheduling decisions in presence of noise, e.g., job failures, commonly observed in production database systems.
  * Delivers provably end-to-end tenant performance isolation with Pareto-optimal SLOs. This is often more desirable than traditional resource isolation.
  * When all SLOs cannot be satisfied—which is common in busy database systems—Tempo guarantees max-min fairness over SLO satisfactions.
  * Adapts to workload patterns and variations.
  * Reduces the risk of major performance regression while being applied to production database systems.

The detailed description of Tempo can be found in our [paper](http://arxiv.org/abs/1512.00757). Implementation of the numerical algorithm is avaiable at: [PALD](https://github.com/ZilongTan/Algorithms/tree/master/PALD).

###Main Components
| Module | |
|--------|:---:|
| script/extractor | Hadoop log analyzer. |
| app/sched_pred | High-performance RM simulator. |
| app/optimizer | Tempo control loop. |
| others | More apps based on Tempo library |

###Requirements
* libconfig: http://www.hyperrealm.com/libconfig/
* ulib: https://code.google.com/p/ulib/
* gsl: http://www.gnu.org/software/gsl/gsl.html
* glpk: http://www.gnu.org/software/glpk/
