args = commandArgs(trailingOnly=TRUE)

indices_to_remove <- c(1, 3) # remove baseName and output directory for comparison
comparison_args <- args[-indices_to_remove]
 
if (length(args)<4)
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
  devtools::install_github("ciat/cleaned@cleaned_v0.6.0")
} else if (packageVersion("cleaned") != '0.6.0') {
  library("devtools")
  devtools::install_github("ciat/cleaned@cleaned_v0.6.0")
}

# load libraries
library(cleaned)
library(jsonlite)
library(ggplot2)

do.call("calculate_differences", as.list(comparison_args))
clean_plotting(args[2], args[3])

