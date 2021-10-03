# Sound System documentation
Meegreef by default uses OpenAL as a backend for sound. On supported platforms[^1], it also makes use of the OpenAL Soft EFX extensions to provide reverb.

 * [Reverb](#reverb)
 * [Sounds.toml](#sounds-toml)

## Reverb
Revereb is calculated every frame, or rather everytime `soundSystem::SetListener` is called.\
To calculate it, the listener's position is taken, and in all 6 cardinal directions, a ray is shot out up to 16 blocks.

Depending on the material of the impact block, the decay time is modified;
| Material | Decay |
| -------- | ----- |
| No Block Hit | 0.05 |
| MAT_NONE | 0.05 |
| MAT_STONE | 0.4 |
| MAT_GLASS | 0.4 |
| MAT_LOOSE | 0.2 |
| MAT_ORGANIC | 0.15 |
| MAT_LIQUID | 0.0 |

These values are added to the decay time via; `decayTime += reverb * ( distanceToBlock / 4.0f )`. The decay time is in seconds.

## Sounds.toml

Sounds are handled with a sounds.toml file. This holds all the definitions the game uses, none are hard-coded. If sounds.toml is missing or invalid, your game output will be a flurry of errors.

The format is as follows:
```toml
[sound.event.name]
type = "Something"
appendSounds = false
sounds = ["sounds/sound1.ogg", "sounds/sound2.ogg", "sounds/sound5.ogg"]
pitch = [0.5,1.5]
```

`type` affects how the game plays the sound, Such as if reverb would apply or if the sound is positional. Has no default value.

 * `block` - The sound comes from a block, such as breaking.
 * `music` - The sound is game music. Not positional or affected by reverb.

`appendSounds` is future-proofing for resource packs, not currently implemented at all. Default of false, but all vanilla sounds mark it manually.

`sounds` is an array of sound filenames relative to the game assets folder, there is no hard-coded limit to this but I'm sure you'll reach one. EVERY sound included in one of these blocks for any sound event is loaded, but only once. Sounds are shared between events.\
Only one may be provided, which would cause no random selection to occur. It's not (currently) valid to just provide a string.\
No default value.

`pitch` is an array of [a,b]. This causes a random pitch between a & b, assuming that A is the lowest. No validation occurs for this though.\
To disable randomisation, a & b should be the same. It's not valid to provide just a single number or an array of one number.\
The default value is [1.0,1.0].


[^1]: Currently only Linux. If the EFX extensions are not available, it is disabled.