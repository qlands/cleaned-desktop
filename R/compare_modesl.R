args = commandArgs(trailingOnly=TRUE)

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
  library("devtools")
  devtools::install_github("ciat/cleaned@v0.3.2")
} else if (packageVersion("cleaned") != '0.3.2') {
  library("devtools")
  devtools::install_github("ciat/cleaned@v0.3.2")
}

# library("devtools")
# devtools::install_github("ciat/cleaned")

# load libraries
library(cleaned)
library(jsonlite)
print(args)
calculate_differences(args)


