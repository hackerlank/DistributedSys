###Tempo - A Hadoop Resource Scheduling Framework with Robust SLA Compliance

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
