TEMPLATE = subdirs

CONFIG += ordered

SUBDIRS = lib \
          cli/launcher \
          cli/problems \
          cli/ssftovdb \
          gui \
          gui/collision \
          gui/rotation \
          gui/nbody \
          gui/reacc \
          gui/player \
          test \
          bench \
          post

run.depends = lib
launcher.depends = lib
problems.depends = lib
ssftovdb.depends = lib
gui.depends = lib
collision.depends = lib gui
player.depends = lib gui
rotation.depends = lib gui
nbody.depends = lib gui
reacc.depends = lib gui
test.depends = lib
bench.depends = lib
