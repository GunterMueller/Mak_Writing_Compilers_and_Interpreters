{************************************************}
{                                                }
{   Turbo Pascal for Windows                     }
{   Tips & Techniques Demo Program               }
{   Copyright (c) 1991 by Borland International  }
{                                                }
{************************************************}

program Animate;

uses WinTypes, WinProcs, WObjects, Strings;

const
  NumberLines = 20;

type
  { Declare Record structure of a lines }
  TLine = Record
    X1, Y1, X2, Y2: Integer;
    Color: LongInt;
  end;

  { Create TDrawWindow, a TWindow decendant}
  PDrawWindow = ^TDrawWindow;
  TDrawWindow = object(TWindow)
    MaxX: Integer;
    MaxY: Integer;
    DX1, DY1, DX2, DY2: Integer;
    LastLine: Integer;
    Lines: array[1..NumberLines] of TLine;
    procedure DrawLine(DC: HDC; Index: Integer; Draw: Boolean);
    procedure GetWindowClass(var WndClass: TWndClass); virtual;
    procedure Next(var A, DA, Max: Integer);
    procedure NextLine;
    procedure WMRight(var Msg: TMessage); virtual WM_RButtonDown;
    procedure Paint(PaintDC: HDC; var PaintInfo: TPaintStruct); virtual;
    procedure SetUpWindow; virtual;
    procedure wmSize(var Message: TMessage);
      virtual wm_Size;
  end;

  { TApp, a decendant of TApplication calls the Idle method
    from the main message loop }
  TApp = object(TApplication)
    procedure Idle; virtual;
    procedure InitMainWindow; virtual;
    procedure MessageLoop; virtual;
  end;

{ Drawline draws a line with the given color }
procedure TDrawWindow.DrawLine(DC: HDC; Index: Integer; Draw: Boolean);
var
  MadeDC: Boolean;
  Pen: HPen;
begin
  MadeDC := DC = 0;
  if MadeDC then
    DC := GetDC(HWindow);
  with Lines[Index] do
  begin
    if Draw then
      Pen := SelectObject(DC, CreatePen(ps_Solid, 1, Lines[Index].Color))
    else
      Pen := SelectObject(DC, CreatePen(ps_Solid, 1, 0));
    MoveTo(DC, X1, Y1);
    LineTo(DC, X2, Y2);
  end;
  DeleteObject(SelectObject(DC, Pen));
  if MadeDC then
    ReleaseDC(HWindow, DC);
end;

procedure TDrawWindow.WMRight(var Msg: TMessage);
begin
  messagebox(hWindow,'asd','asdf',mb_ok);
end;
{ Set background to black }
procedure TDrawWindow.GetWindowClass(var WndClass: TWndClass);
begin
  TWindow.GetWindowClass(WndClass);
  WndClass.HIcon := 0;
  WndClass.hbrBackGround := GetStockObject(Black_Brush);
end;

{ Increment the data structure }
procedure TDrawWindow.Next(var A, DA, Max: Integer);
begin
  Inc(A, DA);
  if A > Max then
  begin
    A := Max;
    DA := ((-1 - Random(3)) * 2);
  end
  else if A < 0 then
  begin
    A := 0;
    DA := (Random(3) + 1) * 2;
  end;
end;

{ Add next line }
procedure TDrawWindow.NextLine;
begin
  DrawLine(0, LastLine, False);
  if LastLine <> 1 then
    Lines[LastLine] := Lines[LastLine - 1]
  else
    Lines[LastLine] := Lines[NumberLines];
  with Lines[LastLine] do
  begin
    Next(X1, DX1, MaxX);
    Next(X2, DX2, MaxX);
    Next(Y1, DY1, MaxY);
    Next(Y2, DY2, MaxY);
    Color := RGB(Random(255), Random(255), Random(255));
  end;
  DrawLine(0, LastLine, True);
  Inc(LastLine);
  if LastLine > NumberLines then
    LastLine := 1;
end;

{ Update the window and call DrawLine}
procedure TDrawWindow.Paint(PaintDC: HDC; var PaintInfo: TPaintStruct);
var
  Index, I: Integer;
begin
  Index := LastLine;
  I := 0;
  while I < NumberLines do
  begin
    DrawLine(PaintDC, Index, True);
    Inc(Index);
    if Index > NumberLines then
      Index := 1;
    Inc(I);
  end;
end;

{ Initialize Random Values }
procedure TDrawWindow.SetUpWindow;
var
  I: Integer;
begin
  TWindow.SetUpWindow;
  Randomize;
  DX1 := (Random(3) + 1) * 2;
  DY1 := (Random(3) + 1) * 2;
  DX2 := (Random(3) + 1) * 2;
  DY1 := (Random(3) + 1) * 2;
  FillChar(Lines, SizeOf(Lines), 0);
  LastLine := 1;
  with Lines[NumberLines] do
  begin
    X1 := Random(100);
    Y1 := Random(100);
    X2 := Random(100);
    Y2 := Random(100);
    Color := 0;
  end;
end;

{ Store object values with current window size }
procedure TDrawWindow.wmSize(var Message: TMessage);
begin
  MaxX := integer(Message.lParamLo);
  MaxY := integer(Message.lParamHi);
end;

{ While message loop is not processing, draw lines }
procedure TApp.Idle;
begin
  PDrawWindow(MainWindow)^.NextLine;
end;

{ Initialize MainWindow with a title }
procedure TApp.InitMainWindow;
begin
  MainWindow := New(PDrawWindow, Init(Nil, 'Animate Window'));
end;

{ Add Idle loop to main message loop.}
procedure TApp.MessageLoop;
var
  Message: TMsg;
begin
  while True do
  begin
    if PeekMessage(Message, 0, 0, 0, pm_Remove) then
    begin
      if Message.Message = wm_Quit then
      begin
        Status := Message.WParam;
        Exit;
      end;
      if not ProcessAppMsg(Message) then
      begin
        TranslateMessage(Message);
        DispatchMessage(Message);
      end;
    end
    else
      Idle;
  end;
end;

var
  App: TApp;
begin
  App.Init('Animate');
  App.Run;
  App.Done;
end.
