unit FormDemoEdit;

{$mode objfpc}{$H+}

interface

uses
  Classes, SysUtils, Forms, Controls, Graphics, Dialogs, ExtCtrls, StdCtrls,
  ComCtrls,DateUtils;

type

  { TForm1 }

  TForm1 = class(TForm)
    Button1: TButton;
    Button2: TButton;
    Button3: TButton;
    Button4: TButton;
    CheckBox1: TCheckBox;
    Label1: TLabel;
    Label2: TLabel;
    Memo1: TMemo;
    Panel1: TPanel;

    Shape00: TShape;
    Shape01: TShape;
    Shape02: TShape;
    Shape03: TShape;
    Shape04: TShape;
    Shape05: TShape;
    Shape06: TShape;
    Shape07: TShape;
    Shape08: TShape;
    Shape09: TShape;
    Shape10: TShape;
    Shape11: TShape;
    Shape12: TShape;
    Shape13: TShape;
    Shape14: TShape;
    Shape15: TShape;
    TrackBar1: TTrackBar;

    procedure Button1Click(Sender: TObject);
    procedure Button2Click(Sender: TObject);
    procedure Button3Click(Sender: TObject);
    procedure Button4Click(Sender: TObject);
    procedure FormCreate(Sender: TObject);
    procedure Shape00MouseDown(Sender: TObject; Button: TMouseButton;
      Shift: TShiftState; X, Y: Integer);
    procedure Shape00MouseEnter(Sender: TObject);
    procedure Shape00MouseLeave(Sender: TObject);
    procedure Shape01MouseEnter(Sender: TObject);
    procedure Shape02MouseEnter(Sender: TObject);
    procedure Shape03MouseEnter(Sender: TObject);
    procedure Shape04MouseEnter(Sender: TObject);
    procedure Shape05MouseEnter(Sender: TObject);
    procedure Shape06MouseEnter(Sender: TObject);
    procedure Shape07MouseEnter(Sender: TObject);
    procedure Shape08MouseEnter(Sender: TObject);
    procedure Shape09MouseEnter(Sender: TObject);
    procedure Shape10MouseEnter(Sender: TObject);
    procedure Shape11MouseEnter(Sender: TObject);
    procedure Shape12MouseEnter(Sender: TObject);
    procedure Shape13MouseEnter(Sender: TObject);
    procedure Shape14MouseEnter(Sender: TObject);
    procedure Shape15MouseEnter(Sender: TObject);

  private
    SelectedBtn : integer;
    light       : array [0..15 ] of boolean;
    LightN      : Word;
    run         : Boolean;

    Procedure SelectBtn;
    Function  LightToNumber : Word;
    Procedure NumberToLight (N:word);
    Procedure RefreshLight;

  public

  end;


var
  Form1: TForm1;
  ShapeArray : array [0..15] of Tshape;

implementation

{$R *.lfm}

{ TForm1 }

procedure RunDelay(millis: int64);
var
  StopAt : TDateTime;
begin
  StopAt := IncMilliSecond(Now,millis);
  while (Now < StopAt) and (not Application.Terminated) do
    Application.ProcessMessages;
end;

Function  TForm1.LightToNumber : Word;
var
  i : integer;
  tmp:word;
begin
  tmp:=0;
  for i:=0 to 15 do
    begin
      if ( light[i] ) then tmp:=tmp or ( 1 << i );
    end;
  LightToNumber:=tmp;
end;

Procedure TForm1.NumberToLight (N:Word);
var
  i : integer;
begin
  for i:=0 to 15 do
  begin
    light[i]:= ( N and ( 1 << i )) > 0;
  end;
end;

Procedure TForm1.RefreshLight;
var
  i : integer;
begin
  for i:=0 to 15 do
  begin
   if  light[i] then ShapeArray[i].Brush.Color:=clWhite
                else ShapeArray[i].Brush.Color:=clAppWorkspace;
    ShapeArray[i].Refresh;
  end;
end;

procedure TForm1.SelectBtn;
begin
  if ( SelectedBtn >=0 ) and ( SelectedBtn <16) then
  begin
    ShapeArray[SelectedBtn].Pen.Width:=2;
    ShapeArray[SelectedBtn].Refresh;
  end;
end;

procedure TForm1.Shape00MouseLeave(Sender: TObject);
begin
  if ( SelectedBtn >=0 ) and ( SelectedBtn <16) then
  begin
    ShapeArray[SelectedBtn].Pen.Width:=1;
    ShapeArray[SelectedBtn].Refresh;
  end;
  SelectedBtn:=-1;

end;

procedure TForm1.Shape00MouseEnter(Sender: TObject);
begin
  SelectedBtn:=0;
  SelectBtn;
end;

procedure TForm1.Shape01MouseEnter(Sender: TObject);
begin
  SelectedBtn:=01;
  SelectBtn;
end;

procedure TForm1.Shape02MouseEnter(Sender: TObject);
begin
  SelectedBtn:=02;
  SelectBtn;
end;

procedure TForm1.Shape03MouseEnter(Sender: TObject);
begin
  SelectedBtn:=03;
  SelectBtn;
end;

procedure TForm1.Shape04MouseEnter(Sender: TObject);
begin
  SelectedBtn:=04;
  SelectBtn;
end;

procedure TForm1.Shape05MouseEnter(Sender: TObject);
begin
  SelectedBtn:=05;
  SelectBtn;
end;

procedure TForm1.Shape06MouseEnter(Sender: TObject);
begin
  SelectedBtn:=06;
  SelectBtn;
end;

procedure TForm1.Shape07MouseEnter(Sender: TObject);
begin
  SelectedBtn:=07;
  SelectBtn;
end;

procedure TForm1.Shape08MouseEnter(Sender: TObject);
begin
  SelectedBtn:=08;
  SelectBtn;
end;

procedure TForm1.Shape09MouseEnter(Sender: TObject);
begin
  SelectedBtn:=09;
  SelectBtn;
end;

procedure TForm1.Shape10MouseEnter(Sender: TObject);
begin
  SelectedBtn:=10;
  SelectBtn;
end;

procedure TForm1.Shape11MouseEnter(Sender: TObject);
begin
  SelectedBtn:=11;
  SelectBtn;
end;

procedure TForm1.Shape12MouseEnter(Sender: TObject);
begin
  SelectedBtn:=12;
  SelectBtn;
end;

procedure TForm1.Shape13MouseEnter(Sender: TObject);
begin
  SelectedBtn:=13;
  SelectBtn;
end;

procedure TForm1.Shape14MouseEnter(Sender: TObject);
begin
  SelectedBtn:=14;
  SelectBtn;
end;

procedure TForm1.Shape15MouseEnter(Sender: TObject);
begin
  SelectedBtn:=15;
  SelectBtn;
end;

procedure TForm1.FormCreate(Sender: TObject);
var
  i : integer;
begin
  ShapeArray[ 0]:=Shape00;
  ShapeArray[ 1]:=Shape01;
  ShapeArray[ 2]:=Shape02;
  ShapeArray[ 3]:=Shape03;
  ShapeArray[ 4]:=Shape04;
  ShapeArray[ 5]:=Shape05;
  ShapeArray[ 6]:=Shape06;
  ShapeArray[ 7]:=Shape07;
  ShapeArray[ 8]:=Shape08;
  ShapeArray[ 9]:=Shape09;
  ShapeArray[10]:=Shape10;
  ShapeArray[11]:=Shape11;
  ShapeArray[12]:=Shape12;
  ShapeArray[13]:=Shape13;
  ShapeArray[14]:=Shape14;
  ShapeArray[15]:=Shape15;

  for i:=0 to 15 do light[i]:=false;
//  Memo1.Lines.Clear;

end;


procedure TForm1.Button1Click(Sender: TObject);
begin
  Memo1.Lines.add(IntToStr(LightN));
end;

procedure TForm1.Button2Click(Sender: TObject);
var
   i   : integer;
   S   : string;
   N   : Word;

begin
  run:=true;
  while run do
    begin
      for i:=0 to Memo1.Lines.Count-1 do if (run ) then
      begin
        s:=Memo1.Lines[i];
        N:=StrToInt(s);
        NumberToLight(N);
        RunDelay(TrackBar1.Position*10);
        RefreshLight;
        Application.ProcessMessages;
      end;
      Application.ProcessMessages;
      if not(checkbox1.Checked) then run:=false;
      if Application.Terminated then run:=false;
    end;
end;

procedure TForm1.Button3Click(Sender: TObject);
begin
  run:=false;
//  checkbox1.Checked:=false;
end;

procedure TForm1.Button4Click(Sender: TObject);
begin
  run:=false;
  self.Close;
end;


procedure TForm1.Shape00MouseDown(Sender: TObject; Button: TMouseButton;
  Shift: TShiftState; X, Y: Integer);
var
  buff : string;
begin
   if ( SelectedBtn >=0 ) and ( SelectedBtn <16) then
  begin
    light[SelectedBtn]:= not (light[SelectedBtn]);
    if  light[SelectedBtn] then ShapeArray[SelectedBtn].Brush.Color:=clWhite
                           else ShapeArray[SelectedBtn].Brush.Color:=clAppWorkspace;
  end;
  LightN:=LightToNumber;
  buff:=format('%d'  , [LightN] ); Label1.Caption:=buff;
  buff:=format('0x%.4x', [LightN] ); Label2.Caption:=buff;

end;



end.

