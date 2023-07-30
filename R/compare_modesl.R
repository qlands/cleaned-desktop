args = commandArgs(trailingOnly=TRUE)

comparison_args <- args[-2]
 
if (length(args)<3)
{
  stop("You need to indicate at least two model result files and a comparison result file", call.=FALSE)
}

.libPaths()

if (!require("devtools"))
{
  install.packages("devtools",repos="http://cran.rstudio.com/")
}

if (!require("cleaned"))
{
  install.packages("ggplot2")
}

if (!require("cleaned"))
{
  library("devtools")
  devtools::install_github("ciat/cleaned@cleaned_v0.4.0")
} else if (packageVersion("cleaned") != '0.3.2') {
  library("devtools")
  devtools::install_github("ciat/cleaned@cleaned_v0.4.0")
}

# load libraries
library(cleaned)
library(jsonlite)
library(ggplot2)

do.call("calculate_differences", as.list(comparison_args))
clean_plotting(args[1], args[2])

