# Limitations
Key design idea of this engine is limitations, I do not want to design a general purpose engine
but rather something limited which forces the user to think outside the box. Besides it's not a
real product...

## Image formats
We only support images of { PNG } format and images are expected to be stored in { assets/images }
directory.

## Entity count
As of this moment, there's no limit to entity count, but in the future ECS refactor/simplification
pass I would like to set a hard limit to max entity count.

## Component type count
Component types are not expected to exceed 16 or 32, which should be more than enough to support
all desired engine features.
