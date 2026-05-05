{
  stdenv,
  fetchFromGitHub,
  makeWrapper,
  cmake,
  sfml,
}:
stdenv.mkDerivation {
  pname = "pooooooooool";
  version = "1.0.0";

  src = fetchFromGitHub {
    owner = "andy-sorge";
    repo = "pooooooooool";
    rev = "v1.0.0";
    sha256 = "EO5MEvGGxwD5lU8JHADwOI1nxYlAi9OuWcfy0x20YbA=";
  };

  nativeBuildInputs = [ cmake makeWrapper ];
  buildInputs = [ sfml ];

  installPhase = ''
    mkdir -p $out/bin $out/share/icons/hicolor/192x192/apps/
    cp -r $src/resource $out/resource
    cp $PWD/pooooooooool $out/bin/
    wrapProgram $out/bin/pooooooooool --run "cd $out/resource"

    cp $src/resource/graphics/balls/8.png $out/share/icons/hicolor/192x192/apps/eightball.png
    # desktop entry
    install -Dm644 <(cat << EOF
    [Desktop Entry]
    Name=Pooooooooool
    Exec=$out/bin/pooooooooool
    Icon=eightball
    Type=Application
    Categories=Game;
    EOF
    ) $out/share/applications/pooooooooool.desktop
  '';
}
