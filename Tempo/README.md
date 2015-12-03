###Tempo - Robust and Self-Tuning Resource Management in Multi-tenant Parallel Databases

Popular cluster resource managers (RMs) today do not provide direct support for performance objectives such as: “Average job response time of tenant A must be less than two minutes”, or “No more than 5% of tenant B’s jobs can miss the deadline of 1 hour.” Thus, DBAs have to tinker with the RM’s low-level configuration settings to meet such objectives.

Tempo is a framework that brings several desirable properties to existing RMs:
* *Simplicity.* Tempo a simple interface for DBAs to specify performance objectives (SLOs) declaratively.
* *Self-tuning.* Tempo adapts to workload patterns and variations.
* *Robustness.*
  * Provable convergence of Tempo to deliver Pareto-optimal resource allocations.
  * Deliver max-min fairness over *SLOs*, when resources are insufficient to meet all performance requirements.
  * Safe to be used in production.

The detailed description of Tempo can be found in our [paper](http://arxiv.org/abs/1512.00757).

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
