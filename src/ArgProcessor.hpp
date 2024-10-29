namespace modulizer {

struct Opts {
  bool merge;
  StrView inDir;
  StrView outDir;
  StrView hdrExtRegex;
  StrView srcExtRegex;
  StrView moduleInterfaceExt;
  StrView moduleImplExt;
  StrView openExport;
  StrView closeExport;
private:
  Str members;
};

Opts getOptsOrExit(int argc, char* argv[], bool& verbose);

}