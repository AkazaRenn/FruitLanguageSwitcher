# Fruit Language Switcher
A language switcher to mimic the famous you-know-who fruit on Windows.

<a href="https://apps.microsoft.com/detail/9njd8g4v1g1k?referrer=appbadge&mode=direct">
	<img src="https://get.microsoft.com/images/en-us%20dark.svg" width="200"/>
</a>

## What it does
### Category-based swtiching
It separates the Windows keyboard layouts `win + space` into two categories: latin
languages and IME languages. Now it would track the both the active latin language and
the IME language to allow you to switch between them with a single click on `CapsLock`
(yes, just like on that fruit).

To toggle the actual `CapsLock` state, hold `CapsLock` key for more than 500ms.

For example as a user with `[en-US, zh-TW, fr-CA, ja-JP]` as the available languages
for the OS.

The startup state of the langauges: \
**`en-US ([en-US, fr-CA])`** `zh-TW ([zh-TW, ja-JP])`

After hitting `CapsLock`: \
`en-US ([en-US, fr-CA])` **`zh-TW ([zh-TW, ja-JP])`**

After switching to `fr-CA`: \
**`fr-CA ([en-US, fr-CA])`** `zh-TW ([zh-TW, ja-JP])`

After hitting `CapsLock`: \
`fr-CA ([en-US, fr-CA])` **`zh-TW ([zh-TW, ja-JP])`**

After switching to `ja-JP`: \
`fr-CA ([en-US, fr-CA])` **`ja-JP ([zh-TW, ja-JP])`**

### Fix conversion mode
It's well-know that Microsoft does something stupid regarding the convertion mode
on IMEs, e.g. setting the conversion mode to Alphanumeric when you have switched to it
from a latin language. It will fix the conversion mode when an IME language is activated.

### RAlt enhancement
The `RAlt` button is replaced by useful functions.

#### Japanese
`RAlt` now swaps between Katakana, Hirakana and half-width Kana modes (done by sending `VK_IME_ON`);

#### Chinese and Korean
`RAlt` now toggles between Alphanumeric mode and convert mode. It still defaults to the convert mode
due to the aforementioend fix.

## Extra
It remaps single `LWin` key press to `LWin + Alt + Space` to bring up `Command Palette` from [`PowerToys`](https://github.com/microsoft/PowerToys). **It will only do so when it detects
`Command Palette` is installed.**
