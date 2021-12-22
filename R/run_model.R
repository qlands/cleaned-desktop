args = commandArgs(trailingOnly=TRUE)

if (length(args)!=5) 
{
  stop("You need to indicate 5 JSON files: input, ghg, stock, energy, and output.n", call.=FALSE)
}

.libPaths()


if (!require("devtools"))
{
  install.packages("devtools",repos="http://cran.rstudio.com/")
}

if (!require("cleaned"))
{
  library("devtools")
  devtools::install_github("ciat/cleaned@v0.2.1")
}

# library("devtools")
# devtools::install_github("ciat/cleaned")

# load libraries
library(cleaned)
library(jsonlite)

#write(args[1], stdout())
#write(args[2], stdout())
#write(args[3], stdout())
#write(args[4], stdout())
#write(args[5], stdout())

# load data
para <- fromJSON(args[1], flatten = TRUE)
ghg_ipcc_data <- fromJSON(args[2], flatten = TRUE)
stock_change_para <- fromJSON(args[3], flatten = TRUE)
energy_parameters <- fromJSON(args[4], flatten = TRUE)

feed_basket_quality <- feed_quality(para)
energy_required <- energy_requirement(para, feed_basket_quality,energy_parameters)
land_required <- land_requirement(feed_basket_quality, energy_required, para)
soil_erosion <- soil_health(para, land_required)
water_required <- water_requirement(para,land_required)
nitrogen_balance <- n_balance(para, land_required, soil_erosion)
livestock_productivity <- land_productivity(para)
economics <- economics_payback(para, energy_required, livestock_productivity)
biomass <- biomass_calculation(para, land_required)
soil_carbon <- soil_organic_carbon(para, stock_change_para, land_required, biomass)
ghg_emissions <- ghg_emission(para, energy_required, ghg_ipcc_data, land_required, nitrogen_balance)
cleanedOut <- combineOutputs(feed_basket_quality, energy_required, land_required, soil_erosion, water_required, nitrogen_balance,livestock_productivity,economics,biomass,soil_carbon,ghg_emissions)

# write output
write(cleanedOut, args[5])
