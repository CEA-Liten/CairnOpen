#================================================================
# cmake utilities to build Cairn component
#================================================================


find_package(MIPModeler REQUIRED)

# suffixe of the model library name
set(MODELS_SFX CairnModel) 

# list of models directories
set(models_DIRS
bus
converter
grid
operationconstraint
sourceload
storage
)

include(ComponentSetup)
create_models(models_DIRS)
