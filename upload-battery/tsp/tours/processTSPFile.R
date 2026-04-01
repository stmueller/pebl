##
## This function can be used to process a TSP save file created by the
## Concorde Windows GUI and create a problem file to be used by the PEBL
## TSP task.  This also scales the problems so that they fit within a
## max 500x500 field (not changing the aspect ratio).
## To use, save a solved problem via the Concorde solver (do not export)
## to some file (e.g., "filename"). 
## Then, use processTSPFile("filename"), with R in the same working directory
## as the file.  This will save the processed file to "filename.csv"
##
## Released into the public domain 8/2014 by Shane Mueller as
## part of the PEBL Project

processTSPFile <- function(filebase, maxscale=500)
    {
        ##maxscale is the maximum size that the points should be scaled to,
        ##keeping the aspect ratio of x and y.
        ##Points are likely to be generated within a 100x100 grid.
        data <- scan(filebase)
        length <- data[1]
        xypoints <-cbind( data[(1+1:length*2)],
                          data[(1+1:length*2)+1])
        ##scale the points:
        xypoints <- xypoints*maxscale/max(xypoints)/1.1


        ##extract the order information:
        order <-matrix( data[((length+1)*2+1):length(data)],
                         ncol=3,byrow=T)
        ord <- order[,1]+1
        newpoints <- data.frame(city=paste("City",order[ord,1]),
                                xypoints[ord,],0)
        plot(xypoints[ord,],type="b")##display the path for verification
        ##save out into a new file.
        out <- file(paste(filebase,".txt",sep=""),"w")
        for(i in 1:nrow(newpoints))
            {
                cat( paste(newpoints[i,1],newpoints[i,2],
                    newpoints[i,3],newpoints[i,4],
                    "\n"),file=out)
            }
        close(out)
    }

