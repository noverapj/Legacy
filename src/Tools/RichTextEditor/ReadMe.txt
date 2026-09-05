- RichTextEditor UI 에디터 환경설정
  1) VC실행 후 속성관리자 -> RichTextEditor 프로젝트 -> DebugSheet/ReleaseSheet -> 사용자 매크로에서 환경변수 설정
     - LostSagaWorkingDir : D:\LostSaga_Dev2\UITool\ (실제 UI툴이 실행될 폴더)
     - LibPath : C:\Project\LProject\LSClient2\programming\lib\ (개발 라이브러리가 참조될 폴더)

  2) 빌드전이벤트 등록
     copy "$(LibPath)io3DEngined.dll" "$(LostSagaWorkingDir)"
     copy "$(LibPath)io3DEngined.lib" "$(LostSagaWorkingDir)"
     copy "$(LibPath)io3DEngined.pdb" "$(LostSagaWorkingDir)"
     copy "$(LibPath)LSLogD.dll" "$(LostSagaWorkingDir)"
     copy "$(LibPath)ioFreeTypeD.dll" "$(LostSagaWorkingDir)"
     copy "$(LibPath)ioPacd.dll" "$(LostSagaWorkingDir)"
     copy "$(LibPath)vorbisfile.dll" "$(LostSagaWorkingDir)"
     copy "$(LibPath)ogg.dll" "$(LostSagaWorkingDir)"
     copy "$(LibPath)vorbis.dll" "$(LostSagaWorkingDir)"
     copy "$(LibPath)IL.dll" "$(LostSagaWorkingDir)"
     copy "$(LibPath)Opcoded.dll" "$(LostSagaWorkingDir)"

  3) 위의 환경 변수가 등록되었다면 작업 폴더에 UI툴과 관련 dll들이 빌드 이벤트에 의해 복사됨