#!/usr/bin/env RScript
library(colormap)
color<-commandArgs(TRUE)
m0<-colormap(color, 16, format="rgb")/16
m1<-matrix(sapply(m0,as.integer),16)[,-4]
m2<-cbind(seq(0,15),m1)
write.table(m2, sep=',', row.names=FALSE, col.names=FALSE)

