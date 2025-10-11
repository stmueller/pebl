dat <- read.table("pathstats.txt")
colnames(dat) <- c("seed","w","h","nodes","dist","anglesum","opts","entropy",paste("NODE",1:49,sep=""))


##remove duplicate seeds
dat2 <- unique(dat)
par(mfrow=c(1,3))
hist(dat$dist,breaks=100,col="navy")
hist(dat$anglesum,breaks=100,col="navy")
hist(dat$entropy,breaks=100,col="navy")

cor(cbind(dat$dist,dat$anglesum,dat$entropy))
pairs(data.frame(Distance=dat$dist+rnorm(nrow(dat))*.2,
                 Angles=dat$anglesum+rnorm(nrow(dat))*.2,
                 Entropy=dat$entropy))



library(hexbin)

bin<-hexbin(dat$dist,dat$anglesum, xbins=50)
plot(bin, main="Hexagonal Binning") 
##3d scatterplot??
library(scatterplot3d)

scatterplot3d(dat$dist,dat$anglesum,dat$entropy,
              pch="X")


library(rgl)
plot3d(dat$dist,dat$anglesum,dat$entropy,col="red", size=3) 



dat$distbin <-ifelse(dat$dist<52.5,1,ifelse(dat$dist>57,3,ifelse(dat$dist>54.6&dat$dist<55,2,NA)))


dat$anglebin <- ifelse(dat$anglesum<60,1,ifelse(dat$anglesum>96,3,ifelse(dat$anglesum>72 & dat$anglesum<77,2,NA)))

dat$entbin <- ifelse(dat$entropy<14.5,1,ifelse(dat$entropy>17.5,3,ifelse(dat$entropy>15.5 & dat$entropy<16.5,2,NA)))


##Let's break each one into quintiles
dat$dist3 <-floor((rank(dat$dist)-1)/length(dat$dist)*3)
dat$ang3 <- floor((rank(dat$anglesum)-1)/length(dat$anglesum)*3)
dat$ent3 <- floor((rank(dat$entropy)-1)/length(dat$entropy)*3)


dat$dist5 <-floor((rank(dat$dist)-1)/length(dat$dist)*5)
dat$ang5 <- floor((rank(dat$anglesum)-1)/length(dat$anglesum)*5)
dat$ent5 <- floor((rank(dat$entropy)-1)/length(dat$entropy)*5)


cont3 <- table(dat$ent3,dat$dist3,dat$ang3)
cont5 <- table(dat$ent5,dat$dist5,dat$ang5)

cont5.3 <- (cont5[c(1,3,5),c(1,3,5),c(1,3,5)])



##Now, let's select candidate RNG seeds.
d <-  dat[!is.na(dat$distbin)&!is.na(dat$anglebin)&!is.na(dat$entbin),]
mmm <- d[d$distbin==2&d$anglebin==2&d$entbin==2,]
hmm <- d[d$distbin==3&d$anglebin==2&d$entbin==2,]
lmm <- d[d$distbin==1&d$anglebin==2&d$entbin==2,]
mhm <- d[d$distbin==2&d$anglebin==3&d$entbin==2,]
mlm <- d[d$distbin==2&d$anglebin==1&d$entbin==2,]
mmh <- d[d$distbin==2&d$anglebin==2&d$entbin==3,]
mml <- d[d$distbin==2&d$anglebin==2&d$entbin==1,]
mmm$seed
