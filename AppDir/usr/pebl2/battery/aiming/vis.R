
dat1 <-  read.csv("data/morning/aiming-morning.csv")
dat2 <-  read.csv("data/22/aiming-22.csv")


dat <- dat2

plot(dat$targ.x,rep(0,nrow(dat)),ylim=c(-400,400),xlim=c(0,1200),pch=1,cex=3,col=as.factor(dat$targ.x))

points(dat$posx,dat$posy-dat$targ.y,col=as.factor(dat$targ.x))
segments(0:12*100,-400,0:12*100,400,lty=3)
segments(0,-4:4*100,1200,-4:4*100,lty=3)
         

tab <- tapply(dat$hit,list(dat$distancecat,dat$targsize),mean)
matplot(rownames(tab),tab,xlab="Distance",ylab="Accuracy",ylim=c(0,1),type="o",
        pch=16)

tab2 <-tapply(dat$delta,list(dat$distancecat,dat$targsize),mean)
matplot(rownames(tab2),tab2,xlab="Distance",ylab="Accuracy",type="o",
        ylim=c(0,max(tab2)),        pch=c("1","2","3"))


dat$delta[dat$delta>200]<-NA
plot(dat$delta)

model <- lm(delta~trial+distance+as.factor(targsize),data=dat)
summary(model)
