

##this does an exact scoring percentage. use a reduced segment structure prior to giving this.
#segments1 and 2 should be lists of nodes.
score.drawing <- function(segments1,segments2)
{
  segs1 <- (segments1)
  segs2 <- (segments2)
  
  ##reorder start/end to be in numerical order, come up with a list of segments with no duplicates
  
  sorted1 <-  unique(sort(apply(segs1,1,function(x){paste(sort(x),collapse="-")})))
  sorted2 <-  unique(sort(apply(segs2,1,function(x){paste(sort(x),collapse="-")})))
  
  
  
  #  sorted1 <- sort(unlist(unique(lapply(segs1[,1:2],function(x){paste(sort(x),collapse="-")}))))
  #  sorted2 <-  sort(unlist(unique(lapply(segs2[,1:2],function(x){paste(sort(x),collapse="-")}))))
  
  total <- union(sorted1,sorted2)
  common <- intersect(sorted1,sorted2)
  extra <- setdiff(sorted2,sorted1)
  missing <- setdiff(sorted1,sorted2)
  
  
  errors <- length(total)-length(common)
  return(c(errorRate=errors/length(total),
           errors=errors,
           total=length(total),
           common=length(common),
           extra=length(extra),
           missing=length(missing))  )
}

##this scores all pairings of drawings with x and y shifts. Not very efficient.
##segments1 and segments2 are the text-string representations of the sequence.
score.drawing.with.translation <- function(drawstring1,drawstring2,reduce=F,grid_type="orthographic",gridsize=20)
{


  ss1<- t(data.frame(segmentString(drawstring1,reduce=reduce,grid_type=grid_type,gridsize=gridsize)))
  ss2<- t(data.frame(segmentString(drawstring2,reduce=reduce,grid_type=grid_type,gridsize=gridsize)))
  

  
  #this calculates the bounds of the grid to match multiple offsets
  grid1 <- stringtosegments(drawstring1,grid_type=grid_type,gridsize=gridsize)
  grid2 <- stringtosegments(drawstring2,grid_type=grid_type,gridsize=gridsize)

    ##we need to adjust grid2 in x and y directions.
  grid1.xrange <- range(grid1$startX,grid1$endX)
  grid1.yrange <- range(grid1$startY,grid1$endY)
  
  grid2.xrange <- range(grid2$startX,grid2$endX)
  grid2.yrange <- range(grid2$startY,grid2$endY)
  
  xminshift <- min(c(0,grid1.xrange - grid2.xrange))
  xmaxshift <- max(c(0,grid1.xrange - grid2.xrange))
  yminshift <- min(c(0,grid1.yrange - grid2.yrange))
  ymaxshift <- max(c(0,grid1.yrange - grid2.yrange))
  
  ##disable this for the current study because we don't need it; 
  ##ss were given the first two segments so their should not be a shifting problem.
  ## or if there is it is their own fault 
  xminshift <- 0
  xmaxshift<-0
  yminshifgt <- 0
  ymaxshift <- 0
  
  scores <- data.frame(xdiff=NA,ydiff=NA,
                       errorRate=rep(NA,length(xminshift:xmaxshift)*length(yminshift:ymaxshift)),
                       errors=NA,
                       total=NA,
                       common=NA,
                       extra=NA,
                       missing=NA
  )
  
  
  
  i <- 0
  for(xdiff in xminshift:xmaxshift)
  {
    for(ydiff in yminshift:ymaxshift)
    {
      i <- i + 1
      ss2.tmp <- data.frame(start= ss2[,1] + ydiff + xdiff*gridsize,
                            end = ss2[,2] + ydiff + xdiff*gridsize)
      
      #      grid2.tmp$startX <- grid2$startX +xdiff
      #      grid2.tmp$endX <- grid2$endX + xdiff
      #      grid2.tmp$startY <- grid2$startY+ydiff
      #      grid2.tmp$endY <- grid2$endY + ydiff


      ##this is what really gets scored:
      #grid2.tmp$start <- grid2$start + ydiff + xdiff*gridsize
      #grid2.tmp$end   <- grid2$end + ydiff + xdiff*gridsize
    #  print(paste("i:",i,xdiff,ydiff))
      
      scores$xdiff[i] <- xdiff
      scores$ydiff[i] <- ydiff
      score <- score.drawing(ss1, ss2.tmp)
      print(score)
      
      scores$errorRate[i] <- score[1]
      scores$errors[i] <- score[2]
      scores$total[i] <- score[3]
      scores$common[i] <- score[4]
      scores$extra[i] <- score[5]
      scores$missing[i] <- score[6]
      

      print(paste("plotting with grid type",grid_type))


      p <- plotDrawing(paste(apply(ss1,1,function(x){paste(x,collapse=";")}),collapse="|"),
                       paste(apply(ss2.tmp,1,function(x){paste(x,collapse=";")}),collapse="|"),
                       paste(xdiff,ydiff,paste(score,collapse="|")),
                       grid_type=grid_type,
                       gridsize=gridsize)
#      print(p)
    }
  }
  scores
  
}



hcf <- function(x, y) {
  while(y) {
    temp = y
    y = x %% y
    x = temp
  }
  return(x)
}

reduceSegment <- function(start,end,size=20,plot=F)
{
  
  startX <-start %/% size
  startY <- start %% size
  endX   <-  end %/% size 
  endY   <-  end  %% size 
  
  
  
  
  deltaX <- endX-startX
  deltaY <- endY-startY
  
  if(deltaX==0 | deltaY == 0)
  {
    ##must handle negative and positive steps
    step <- c(deltaX,deltaY) [which.max(abs(c(deltaX,deltaY)))]
    
  } else{
    
    #divX <- deltaX %/% 1:deltaX 
    #divY <- deltaY %/% 1:deltaY
    
    step <- hcf(deltaX,deltaY)
  }
  
  step <- hcf(deltaX,deltaY)
  ##How many subsegments can we divide this line into?
  stepslope<- c(deltaX,deltaY)/step  #divide by the hcf of the two elements to get an irreducible integer slope.
  #reps <- c(deltaX,deltaY)/stepslope
  ##if it is 3,1, you want 03-01,
  ##If it is 3-0, you want 0123-0000
  ##if it is 6,4, you want 036-024
  valuemat <-t((c(startX,startY)) + t((0:step) %*% t(stepslope) ) )
  #  nsteps <- max(stepslope)/step + 1
  
  if(plot)
  {
    #We can divide the segments into steps along
    plot(rep(0:size,each=size+1),rep(0:size,size+1),pch=16,col="grey")
    points(c(startX,endX),c(startY,endY),type="o",pch=16)
    
    points(valuemat[,1],valuemat[,2],col="red",cex=2)
  }
  
  
  indexes <-(valuemat[,1])*size + valuemat[,2]
  if(length(indexes)==2)
  {
    segments <- list(indexes)
  } else
  {
    segments <- list()
    for(i in 1:abs(step))
    {
      segments[[i]] <- c(indexes[i],indexes[i+1])
    }
  }
  segments
}


##reduceSegment(66,96,plot=T) #up 10 right 1
#reduceSegment(66,126,plot=T) ##right 3
#reduceSegment(66,129,plot=T) ##up3right3
#reduceSegment(66,128,plot=T) ##up2right3
#reduceSegment(66,188,plot=T) ##right6up2 -> steps of 1,3
#reduceSegment(124,64,plot=T)
is.even <- function(x){x%%2==0}

stringtosegments <- function(drawstring, grid_type="orthographic", gridsize=20)
{
  
  points <- strsplit(drawstring,"[|]")
  segments <- data.frame(start=NA,end=NA)
  for(i in 1:length(points[[1]]))
  {
    split <- as.numeric(strsplit(points[[1]][i],"[;]")[[1]])

    segments[i,] <- split

  }
  segments$startX <-segments[,1] %/% gridsize
  segments$startY <- segments[,1] %% gridsize
  segments$endX   <-  segments[,2] %/% gridsize
  segments$endY   <-  segments[,2] %% gridsize

  ## Apply isometric visual shift for alternating columns
  if(grid_type == "isometric") {
    ##if x and y are even, shift y + .5
    
 if(1)
 {
    ##odd-y/evenX needs to adjust x down .5        
    oddYStart <-  (!is.even(segments$startY)) 
    segments$startX[oddYStart] <- segments$startX[oddYStart] + .5
#    evenoddStart <- !is.even(segments$startX) & 
    oddYEnd <-  (!is.even(segments$endY)) 
    segments$endX[oddYEnd] <- segments$endX[oddYEnd] + .5
  }
}
  segments
}

segmentString <- function(drawstring,reduce=TRUE,plot=F,grid_type="orthographic",gridsize=20)
{
  segs <- stringtosegments(drawstring,grid_type=grid_type,gridsize=gridsize)

  seglist <- list()
  for(i in 1:nrow(segs))
  {
    if(reduce)
      element <- reduceSegment(segs$start[i],segs$end[i],plot=plot)
    else
      element <- list(c(segs$start[i],segs$end[i]))
    seglist <- c(seglist,element)

  }

  return(seglist)
}


plotDrawing <- function(drawstring1,drawstring2,label="",grid_type="orthographic",gridsize=20)
{
  seg1 <- stringtosegments(drawstring1,grid_type=grid_type,gridsize=gridsize)
  seg2 <- stringtosegments(drawstring2,grid_type=grid_type,gridsize=gridsize)
  p <- plotGriddedDrawing(seg1,seg2,label=label)
  return (p)

}


plotGriddedDrawing <- function(seg1,seg2,label="", grid_type=grid_type)
{
  #  seg1 <- stringtosegments(drawstring1)
  seg2 <- as.data.frame(t(t(seg2)+c(0,0,.2,.2,.2,.2)))
  seg1$version <- "Standard"
  seg2$version <- "Drawn"
  seg1$size=.4
  seg2$size=.5
  seg1$alpha <- .8
  seg2$alpha <- .4
  both <- bind_rows(seg1,seg2)   
  #print(both)
  p <- ggplot(both,aes(y=startX,x=startY,yend=endX,xend=endY,color=version)) +
    geom_segment(aes(size=size)) +
    geom_point(aes(y=endX,x=endY))+
    scale_size(range = c(0.5, .8))+
    scale_alpha_manual(name = "version", values = c(.1, .4))+
    theme_minimal() + coord_fixed(ratio=1)+ theme(legend.position="none")+
    scale_x_continuous(breaks=1:20) + scale_y_reverse(breaks=1:20)+
    theme(panel.grid.minor=element_blank()) + ggtitle(label) 
  return(p) 
}

