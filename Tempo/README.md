###Tempo - Robust and Self-Tuning Resource Management in Multi-tenant Parallel Databases

Tempo brings three properties to existing resource managers like Mesos and YARN: simplicity, self-tuning, and robustness. First, Tempo provides a simple inter-
face for DBAs to specify SLOs declaratively. Thus, Tempo
enables the RM to be made aware of SLOs such as: “Average
job response time of tenant A must be less than two min-
utes”, and “No more than 5% of tenant B’s jobs can miss the
deadline of 1 hour.” Second, Tempo constantly monitors the
SLO compliance in the database, and adaptively optimizes
the RM configuration settings to maximize SLO compliance.
Third, Tempo has a solid theoretical foundation which gives
five critical robustness guarantees:
1) Tempo’s optimization and modeling algorithms account
for the noisy nature of production database systems.
2) Tempo’s optimization algorithm converges provably to
a Pareto-optimal RM configuration given that satisfying
multiple tenant SLOs is a multi-objective optimization
problem.
3) When all SLOs cannot be satisfied—which is common


###Features
* Provable SLA compliance
* Support for user-defined high-level SLAs
* Safe in production: adaptive tuning of scheduler configurations

###Main Components
| Module | |
|--------|:---:|
| script/extractor | Hadoop job history parser |
| app/sched_pred | Task schedule predictor |
| app/optimizer | Fair Scheduler configuration optimizer |
| others | More apps based on Tempo library |

###Requirements
* libconfig: http://www.hyperrealm.com/libconfig/
* ulib: https://code.google.com/p/ulib/
* gsl: http://www.gnu.org/software/gsl/gsl.html
* glpk: http://www.gnu.org/software/glpk/
