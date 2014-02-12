export JAVA_HOME=/usr/java/latest

export HADOOP_HOME=/usr/lib/hadoop
export HADOOP_HDFS_HOME=/usr/lib/hadoop-hdfs

CLASSPATH=./
for f in $HADOOP_HOME/*.jar; do
  CLASSPATH=${CLASSPATH}:$f;
done
for f in $HADOOP_HOME/lib/*.jar; do
  CLASSPATH=${CLASSPATH}:$f;
done
for f in $HADOOP_HDFS_HOME/*.jar; do
  CLASSPATH=${CLASSPATH}:$f;
done
for f in $HADOOP_HDFS_HOME/lib/*.jar; do
  CLASSPATH=${CLASSPATH}:$f;
done
  CLASSPATH=${CLASSPATH};

export CLASSPATH

export LD_LIBRARY_PATH=$JAVA_HOME/jre/lib/amd64/server:$LD_LIBRARY_PATH
