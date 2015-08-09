# the-chess-game

atm, there is only one solution - simple desktop app which allows 2 players to play with each other.
implemented with Qt5.4.2 with widgets.
current issues: no images for figures, no normal gameplay (mouse button click event is not handled well => key board space key is used), not all rules are implemented (no check, no checkmate, no game ending at all, no restrictions on check state for King move, no jujube, no castling, no possibility to retreive figure when pawn has come to opposite board edge), consider enhancing mechanism of saving/loading chess game.
some obvious enhancements to be done: add login dialog and database to track players and results.

plans so far:
1. rewrite with WPF on .NET; db creation code-first with EntityFramework; UI (obviously) with XAML
2. make it win store app
3. make it web app with ASP.NET

however, who knows when the plans will be implemented

time tracking:
1. initial Qt app was created (07-08-2015 to 09-08-2015) in 8.5h (Fri - 4h, Sat - 3h, Sun - 1.5h).
