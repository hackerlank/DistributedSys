#!/bin/bash
set -e -x
cat > /etc/hadoop/conf/mapred-site.xml <<EOF
<property>
  <name>yarn.resourcemanager.scheduler.class</name>
  <value>org.apache.hadoop.yarn.server.resourcemanager.scheduler.fair.FairScheduler</value>
</property>
EOF
