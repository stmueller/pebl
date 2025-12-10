dat <- read.table("dext-0.dat",header=T)

plot(log(dat$time)~dat$noise)
plot(log(dat$time),dat$distance)


dat.agg <- aggregate(log(dat$time),list(dat$noise),mean)
plot(log(dat$time)~dat$noise)
points(dat.agg,pch=16,cex=2)
