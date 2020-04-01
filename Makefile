# Copyright 2020 Robert Newgard
#
# This file is part of CxxFrames.
#
# CxxFrames is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# CxxFrames is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with CxxFrames.  If not, see <https://www.gnu.org/licenses/>.

# -- make environment ----------------------------------------------------------
undefine FALSE
undefine NULL

SHELL    := /bin/bash
PHONYS   := $(NULL)
CLEANS   := $(NULL)

SP       := $(shell printf "\x20")
TRUE     := true

# get_list <path> - returns contents of existing file at <path>; otherwise returns ""
# get_str  <path> - returns first string from existing file at <path>; otherwise returns ""
# get_bool <path> - returns "1" if existing file at <path> contains "1"; otherwise returns ""
get_list = $(strip $(file < $(strip $(1))))
get_str  = $(firstword $(call get_list,$(strip $(1))))
get_bool = $(filter 1,$(call get_str, $(strip $(1))))


# -- project -------------------------------------------------------------------
TMP  := tmp
CFG  := cfg

$(shell if [ ! -d "$(TMP)" ] ; then (set -x ; mkdir -p $(TMP)) ; fi)


# -- C++ Environment -----------------------------------------------------------
CXX_VER   := -x c++ -std=c++11
CFLAGS    := -Wall -m64 -g -pthread -fPIC -I .
DFLAGS    := $(NULL)


# -- CxxFrames Library ---------------------------------------------------------
CF_LIB_NAME  := CxxFrames
CF_OBJ_DIR   := $(TMP)/libobj
CF_CFG       := $(CFG)/Lib
CF_OBJ_NAMS  := $(shell ls $(CF_CFG))
CF_LIB_REQS  := $(foreach OBJ,$(CF_OBJ_NAMS),$(CF_OBJ_DIR)/$(OBJ).o)
CF_LIB_TARG  := lib$(CF_LIB_NAME).so

$(shell if [ ! -d "$(CF_OBJ_DIR)" ] ; then (set -x ; mkdir -p $(CF_OBJ_DIR)) ; fi)

define compile-for-obj
    mkdir -p $(CF_OBJ_DIR)
    g++ $(CXX_VER) -c $(DFLAGS) $(CFLAGS) -o $@ $<
endef

define link-for-shared-obj
    g++ -shared -Wl,-soname,$(CF_LIB_TARG) -o $@ $^
endef

define lib-obj-targs
    $(CF_OBJ_DIR)/$(1).o : $(1).cxx $(call get_list,$(CF_CFG)/$(1)/deps) ; $$(call compile-for-obj)
endef

$(if $(FALSE),$(info CF_OBJ_NAMS: $(CF_OBJ_NAMS)))
$(if $(FALSE),$(info CF_LIB_REQS: $(CF_LIB_REQS)))

$(foreach NAM,$(CF_OBJ_NAMS),$(eval $(call lib-obj-targs,$(NAM))))


# -- Apps ----------------------------------------------------------------------
APP_CFG       := $(CFG)/App
APP_EXE_NAMS  := $(shell ls $(APP_CFG))
APP_EXE_REQS  := $(CF_LIB_TARG)

define compile-for-exe
    g++ $(CXX_VER) $(DFLAGS) $(CFLAGS) -L . -o $@ $< -l$(CF_LIB_NAME) -lpcap
endef

define app-exe-targs
    $(1) : $(1).cxx $(call get_list,$(APP_CFG)/$(1)/deps) $(APP_EXE_REQS) ; $$(call compile-for-exe)
endef

$(if $(FALSE),$(info APP_EXE_NAMS: $(APP_EXE_NAMS)))

$(foreach NAM,$(APP_EXE_NAMS),$(eval $(call app-exe-targs,$(NAM))))


# -- Hints ---------------------------------------------------------------------
HINTS_TF := %-17s

define hints_def
    printf "$(HINTS_TF) %s\n" "$(strip $(1))" "$(strip $(2))" ;
endef

define hints_h1_def
    $(call hints_def,target,description)
endef

define hints_h2_def
    $(call hints_def,------------,--------------------------------------------------------)
endef

define top_hints_def
    @ $(hints_h1_def)
    @ $(hints_h2_def)
    @ $(call hints_def , show-cfg          , Show build config - aka 'sc'                     )
    @ $(call hints_def , lib               , Create library $(CF_LIB_TARG) from source        )
    @ $(call hints_def , lib-clean         , Remove library $(CF_LIB_TARG) and $(CF_OBJ_DIR)/ )
    @ $(call hints_def , apps              , Create executables $(APP_EXE_NAMS)               )
    @ $(call hints_def , apps-clean        , Remove executables                               )
    @ $(call hints_def , clean             , Remove library, executables and $(TMP)/          )
endef


# -- Phonys --------------------------------------------------------------------
define phonys_def
    nil
    hints
    show-cfg
    sc
    lib
    lib-clean
    clean
endef
PHONYS += $(strip $(phonys_def))


# -- Cleans --------------------------------------------------------------------
CLEANS += lib-clean
CLEANS += apps-clean


# -- rules ---------------------------------------------------------------------
nil             : $(NULL)         ; @true
hints           : $(NULL)         ; $(top_hints_def)
show-cfg        : $(NULL)         ; bin/cfg-show
sc              : show-cfg        ; $(NULL)
$(CF_LIB_TARG)  : $(CF_LIB_REQS)  ; $(link-for-shared-obj)
lib             : $(CF_LIB_TARG)  ; $(NULL)
lib-clean       : $(NULL)         ; rm -rf $(CF_OBJ_DIR)/* $(CF_LIB_TARG)
apps            : $(APP_EXE_NAMS) ; $(NULL)
apps-clean      : $(NULL)         ; rm -rf $(APP_EXE_NAMS)
clean           : $(CLEANS)       ; rm -rf $(TMP)

.PHONY          : $(PHONYS)

.DEFAULT_GOAL := hints
