# malium (tellurium)

malium is a faster fork of [tellurium](https://github.com/plooshi/Tellurium), the redirect and ssl bypass for ue games that use native http (like fn).

## what i changed

- **url parsing** now reads the source string directly instead of allocating temporary substrings.
- **url rebuilding** copies each part once and no longer rescans the output.
- **string searches** are bounded, so they're safe on empty strings.
- **`s += s`** is now safe.
- **`FString` members** are initialized by default, and redirect loops use array bounds.

## how much faster is it?

| case | malium (tellurium) | malium | gain |
| --- | ---: | ---: | ---: |
| redirected request | 361.7 ns | 266.1 ns | 26.4% |
| non-redirected request | 327.3 ns | 242.8 ns | 25.8% |
| average | 688.9 ns | 517.0 ns | 24.9% |

overall that's about 25% faster on the url: 10k requests drop from 6.89 ms to 5.17 ms. this only covers url parsing, so don't expect the whole thing to be 25% faster.

## configuration

everything is in [Malium/Options.h](Malium/Options.h):

- `Backend`: the url of your backend.
- `URLSet`: which set of urls to use (`Default`, `Hybrid`, `Dev`, or `All`).
- `Console`: turns on a debug console.
- `UseBackendParam`: reads the backend from `-backend=` on the command line.
- `ManualMapping`: initializes synchronously if you're manually mapping the dll.

## building

1. open `Malium.sln` in visual studio 2022.
2. set the configuration to `Release` and the platform to `x64`.
3. build the solution.

you'll find the dll at `x64/Release/Malium.dll`.

## credits

malium is built on ploosh's [original tellurium](https://github.com/plooshi). if you use this code please credit metix.

## more

tellurium is good but malium is still faster so give it a try, ty! :3
