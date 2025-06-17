unit unitprincipal;

{$mode objfpc}{$H+}

interface

uses
  Classes, SysUtils, FileUtil, TAGraph, TASeries, Forms, Controls, Graphics,
  Dialogs, StdCtrls, ActnList, ExtCtrls, Grids, Spin, ComCtrls, Menus, UnitMat,
  unitcontrol, UnitAjuda, leoufscarfoc;

type

  { TFrmMainFrm }

  TFrmMainFrm = class(TForm)
    ButCalc2: TButton;
    ButOpenMat: TButton;
    ButRodaCalc: TButton;
    ButGoMomCurv: TButton;
    ButCalcular: TButton;
    ButSaveResultsELS: TButton;
    ButMomCurvSv: TButton;
    ButELUSv: TButton;
    Chart1: TChart;
    DlgSave: TSaveDialog;
    DlgOpen: TOpenDialog;
    Label26: TLabel;
    Label27: TLabel;
    Label28: TLabel;
    SeriePonto: TLineSeries;
    CheckBoxArmadAtiv: TCheckBox;
    CheckBoxArmadPasiv: TCheckBox;
    EdtELUMxx: TEdit;
    EdtELUMyy: TEdit;
    EdtProcResult: TEdit;
    EdtELUNd: TEdit;
    EdtMomCurvNd: TEdit;
    EdtAlfaELS: TEdit;
    EdtELSMyy: TEdit;
    EdtProcEps1: TEdit;
    EdtNmax: TEdit;
    EdtNmin: TEdit;
    EdtProcEps2: TEdit;
    EdtProcAlfa: TEdit;
    EdtELSNd: TEdit;
    EdtELSMxx: TEdit;
    Label19: TLabel;
    Label21: TLabel;
    Label22: TLabel;
    Label23: TLabel;
    Label24: TLabel;
    Label25: TLabel;
    Label32: TLabel;
    LabelArmAtiv: TLabel;
    LabelChartYSup: TLabel;
    Label11: TLabel;
    Label12: TLabel;
    Label13: TLabel;
    Label14: TLabel;
    Label15: TLabel;
    Label16: TLabel;
    Label17: TLabel;
    Label18: TLabel;
    LabelChartX: TLabel;
    LabelChartYInf: TLabel;
    LabelStatusELU: TLabel;
    LabelStatusELS: TLabel;
    LabelStatusProcurar2: TLabel;
    MainMenuPrincipal: TMainMenu;
    MemoELU: TMemo;
    MemoELS: TMemo;
    MemoProcurar2: TMemo;
    MenuArquivo: TMenuItem;
    MenuAjuda: TMenuItem;
    MenuItem1: TMenuItem;
    MenuItemHifen1: TMenuItem;
    MenuItemOpen: TMenuItem;
    MenuItemNew: TMenuItem;
    MenuItemMateriais: TMenuItem;
    MenuItemConfig: TMenuItem;
    MenuItemSaveAs: TMenuItem;
    MenuMateriais: TMenuItem;
    MenuConfig: TMenuItem;
    PageControl1: TPageControl;
    PlanArmAtiv: TStringGrid;
    ProgressBarELU: TProgressBar;
    ProgressBarELS: TProgressBar;
    Serie1: TLineSeries;
    Label5: TLabel;
    LabelArmPas: TLabel;
    PlanPolyg: TStringGrid;
    SpnEdtArmPas: TSpinEdit;
    SpnEdtArmAtiv: TSpinEdit;
    SpnPolyg: TSpinEdit;
    PlanArmPas: TStringGrid;
    StatusBar1: TStatusBar;
    MainSheetELU: TTabSheet;
    MainSheetELS: TTabSheet;
    MainSheetProcurar: TTabSheet;
    MainSheetProcurar2: TTabSheet;
    procedure ButCalc2Click(Sender: TObject);
    procedure ButELUSvClick(Sender: TObject);
    procedure ButMomCurvSvClick(Sender: TObject);
    procedure ButOpenMatClick(Sender: TObject);
    procedure ButGoELU(Sender: TObject);
    procedure ButGoMomCurvClick(Sender: TObject);
    procedure ButProcurarClick(Sender: TObject);
    procedure ButSaveResultsELSClick(Sender: TObject);
    procedure CheckBoxArmadAtivChange(Sender: TObject);
    procedure CheckBoxArmadPasivChange(Sender: TObject);
    procedure FormCreate(Sender: TObject);
    procedure MenuItem1Click(Sender: TObject);
    procedure MenuItemConfigClick(Sender: TObject);
    procedure MenuItemMateriaisClick(Sender: TObject);
    procedure MenuItemOpenClick(Sender: TObject);
    procedure MenuItemSaveAsClick(Sender: TObject);
    procedure SpnEdtArmAtivChange(Sender: TObject);
    procedure SpnEdtArmPasChange(Sender: TObject);
    procedure SpnPolygChange(Sender: TObject);
  private
    { private declarations }
  public
    { public declarations }
    procedure SalvarArquivo(strsave:TFilename);
    procedure AbrirArquivo(stropen:TFilename);
  end;

var
  FrmMainFrm: TFrmMainFrm;
  CoordSec,CoordArmPas,CoordArmAtiv,CoordSecT,CoordArmPasT,CoordArmAtivT: array of Tcoord;
  fVarredura,fModoDeRuptura,fModoDeRupturaDefault:integer;
  fStrtemp:String;
  fVarBin,fbArmPas,fbArmAtiv:Boolean;
  fAc,fAs,fAp,fxcg,fycg,fNdmax,fNdmin,fNdtemp,fNdFixado,
    fvarMtemp,fMtempx,fMtempy,fMx,fMy,
    fSigmacd,ffck,ffcd,fGamaC,ffyk,ffyd,fEs,fEpsilonYD,fGamaS,
    ffpyd,ffptd,fEp,fEpsilonpu,fEpsilonpyd,fEtaP1,
    fvmax,fvmin,fhalfa,fAvmax,fAvmin,fdalfa,fELUalfa,falfatemp,fxalfa,
    fEpsilon1,fEpsilon2,fDeltaEps,
    fEpsilonC2,fEpsilonCU,fEpsilonsu,fxEpsilonC2,fNTensaoConcreto,
    fCurvatura,fEpsilonErroELU,fEpsilonErroMomCurv,fEpsilonErroELS,fEpsilonELSF,varTemp:real;// Declaração de todas variaveis reais
  fTemp:integer;
  aAs,ahsi,aEpsilonsi,aAp,ahpi,aEpsilonprealong,aEpsilonpsi,aDiametroAp:array of real;


implementation
{$R *.lfm}

{ TFrmMainFrm }


// ARQUIVO COM FUNCOES: LEOUFSCARFOC

procedure AtualizarStringGridPolyg(var PlanPontos:TStringGrid;SpnEdt:TSpinEdit);
var
  Variavelfeliz:integer;

begin
     try
       PlanPontos.RowCount:=SpnEdt.Value+1;

     finally
        for Variavelfeliz:=1 to SpnEdt.Value do
        begin
            PlanPontos.Cells[0,Variavelfeliz]:=FloatToStr(Variavelfeliz);
        end;

    end;

end;

procedure VetorReceberPontos(const Plan:TStringGrid; var Coord:array of Tcoord);
                 {Lembrar de sempre modificar o tamanho da array of TCoord antes de chamar essa função}
var
  iContador:integer;

begin

  for iContador:=1 to Plan.RowCount-1 do
  begin
      Coord[iContador-1].xi:=StrToFloat(Plan.Cells[1,iContador]);
      Coord[iContador-1].yi:=StrToFloat(Plan.Cells[2,iContador]);
      Coord[iContador-1].npol:=iContador-1;
  end;

  Coord[iContador].xi:=Coord[0].xi;
  Coord[iContador].yi:=Coord[0].yi ;
  Coord[iContador].npol:=iContador;

end;

procedure VetorReceberPontosArmaduraPassiva(const ArmPasPlan:TStringGrid;var paAs:array of real ;var CoordArmPas:array of Tcoord);
                 {Lembrar de sempre modificar o tamanho da array of TCoord e REAL antes de chamar essa função}
var
  iContador:integer;

begin
  for iContador:=1 to ArmPasPlan.RowCount-1 do
  begin
      CoordArmPas[iContador-1].xi:=StrToFloat(ArmPasPlan.Cells[1,iContador]);
      CoordArmPas[iContador-1].yi:=StrToFloat(ArmPasPlan.Cells[2,iContador]);
      CoordArmPas[iContador-1].npol:=iContador-1;
      paAs[iContador-1]:=StrToFloat(ArmPasPlan.Cells[3,iContador]);

  end;
end;

procedure VetorReceberPontosArmaduraAtiva(const ArmAtivPlan:TStringGrid;var paAp,paEpsilonprealong,paDiametroAp:array of real ;var CoordArmAtiv:array of Tcoord);
                 {Lembrar de sempre modificar o tamanho da array of TCoord e REAIS antes de chamar essa função}
var
  iContador:integer;

begin
  for iContador:=1 to ArmAtivPlan.RowCount-1 do
  begin
      CoordArmAtiv[iContador-1].xi:=StrToFloat(ArmAtivPlan.Cells[1,iContador]);
      CoordArmAtiv[iContador-1].yi:=StrToFloat(ArmAtivPlan.Cells[2,iContador]);
      CoordArmAtiv[iContador-1].npol:=iContador-1;
      paAp[iContador-1]:=StrToFloat(ArmAtivPlan.Cells[3,iContador]);
      paEpsilonprealong[iContador-1]:=-StrToFloat(ArmAtivPlan.Cells[4,iContador]);
      paDiametroAp[iContador-1]:=StrToFloat(ArmAtivPlan.Cells[5,iContador]);
  end;
end;

procedure TFrmMainFrm.ButELUSvClick(Sender: TObject);
begin
  if DlgSave.Execute then MemoELU.Lines.SaveToFile(DlgSave.FileName);
end;

procedure TFrmMainFrm.ButMomCurvSvClick(Sender: TObject);
begin
  if DlgSave.Execute then MemoELS.Lines.SaveToFile(DlgSave.FileName);
end;


procedure TFrmMainFrm.ButSaveResultsELSClick(Sender: TObject);
begin
  if DlgSave.Execute then MemoProcurar2.Lines.SaveToFile(DlgSave.FileName);
end;

procedure TFrmMainFrm.FormCreate(Sender: TObject);
begin
  CheckBoxArmadAtivChange(CheckBoxArmadAtiv);
  CheckBoxArmadPasivChange(CheckBoxArmadPasiv);
  ffck:=30;
  fGamaC:=1.4;
  ffcd:=ffck/fGamaC/10;
  ffyk:=500;
  fGamaS:=1.15;
  fEs:=21000;
  ffyd:=ffyk/fGamaS/10;
  fEpsilonYD:=ffyd/fEs*1000;  // As deformações estão sendo calculadas considerando 1/1000
  fEpsilonsu:=-10;

  ffpyd:=-146;
  ffptd:=-162.6;
  fEp:=20000;
  fEpsilonpu:=-35;
  fEpsilonpyd:=ffpyd/fEp*1000;  // As deformações estão sendo calculadas considerando 1/1000
  fEtaP1:=1.2;

  LabelStatusELU.Caption:='Status: Ocioso';
  fELUalfa:=3;
  fEpsilonErroELU:=0.00001;
  fEpsilonErroMomCurv:=0.00001;
  fEpsilonErroELS:=0.00001;
  fEpsilonELSF:=0.15; // DEFORMACAO MAXIMA DE TRACAO PARA ELS-F
  fModoDeRuptura:=1;
  fModoDeRupturaDefault:=1;

  if ffck<=50 then begin
    fEpsilonC2:=2;
    fEpsilonCU:=3.5;
    fNTensaoConcreto:=2;
  end
  else begin
    fEpsilonC2:=2+0.085*exp(ln(ffck-50)*0.53);
    fEpsilonCU:=2.6+35*((90-ffck)/100)*((90-ffck)/100)*((90-ffck)/100)*((90-ffck)/100);
    fNTensaoConcreto:=1.4+23.4*((90-ffck)/100)*((90-ffck)/100)*((90-ffck)/100)*((90-ffck)/100);
  end;
  StatusBar1.Panels[0].Text:='fck = '+floattostr(ffck)+' MPa';       // Ao modificar os painéis aqui, deve-se modifica-los também no botão Ok, do outro form Materiais
  StatusBar1.Panels[1].Text:='fcd = '+floattostrf(ffcd,ffGeneral,5,3)+' kN/cm²';
  StatusBar1.Panels[2].Text:='n = '+floattostrf(fNTensaoConcreto,ffGeneral,5,3);
  StatusBar1.Panels[3].Text:='Deformações c2 e cu = '+floattostrf(fEpsilonC2,ffGeneral,5,3)+' e '+floattostrf(fEpsilonCU,ffGeneral,5,3);
  StatusBar1.Panels[4].Text:='fyk = '+floattostr(ffyk)+' MPa';
  StatusBar1.Panels[5].Text:='fyd = '+floattostrf(ffyd,ffGeneral,5,3)+' kN/cm²';
  StatusBar1.Panels[6].Text:='Es = '+floattostrf(fEs,ffGeneral,5,3)+' kN/cm²';
  StatusBar1.Panels[7].Text:='Deformação Limite do Aço = '+floattostrf(fEpsilonYD,ffGeneral,5,3);
end;

procedure TFrmMainFrm.MenuItem1Click(Sender: TObject);
begin
  FrmSobre.ShowModal;
end;

procedure TFrmMainFrm.MenuItemConfigClick(Sender: TObject);
begin
   FrmConfig.ShowModal;
end;

procedure TFrmMainFrm.MenuItemMateriaisClick(Sender: TObject);
begin
  Materiais.ShowModal;

end;

procedure TFrmMainFrm.MenuItemOpenClick(Sender: TObject);
begin
  if DlgOpen.Execute then begin
    AbrirArquivo(DlgOpen.filename);
    CheckBoxArmadAtivChange(CheckBoxArmadAtiv);
    CheckBoxArmadPasivChange(CheckBoxArmadPasiv);
    ffcd:=ffck/fGamaC/10;
    ffyd:=ffyk/fGamaS/10;
    fEpsilonYD:=ffyd/fEs*1000;  // As deformações estão sendo calculadas considerando 1/1000
    fEpsilonpyd:=ffpyd/fEp*1000;  // As deformações estão sendo calculadas considerando 1/1000
    if ffck<=50 then begin
      fEpsilonC2:=2;
      fEpsilonCU:=3.5;
      fNTensaoConcreto:=2;
    end;
    if ffck>50 then begin
      fEpsilonC2:=2+0.085*exp(ln(ffck-50)*0.53);
      fEpsilonCU:=2.6+35*((90-ffck)/100)*((90-ffck)/100)*((90-ffck)/100)*((90-ffck)/100);   //Elevado a quarta potencia
      fNTensaoConcreto:=1.4+23.4*((90-ffck)/100)*((90-ffck)/100)*((90-ffck)/100)*((90-ffck)/100);
    end;
    StatusBar1.Panels[0].Text:='fck = '+floattostr(ffck)+' MPa';       // Ao modificar os painéis aqui, deve-se modifica-los também no botão Ok, do outro form Materiais
    StatusBar1.Panels[1].Text:='fcd = '+floattostrf(ffcd,ffGeneral,5,3)+' kN/cm²';
    StatusBar1.Panels[2].Text:='n = '+floattostrf(fNTensaoConcreto,ffGeneral,5,3);
    StatusBar1.Panels[3].Text:='Deformações c2 e cu = '+floattostrf(fEpsilonC2,ffGeneral,5,3)+' e '+floattostrf(fEpsilonCU,ffGeneral,5,3);
    StatusBar1.Panels[4].Text:='fyk = '+floattostr(ffyk)+' MPa';
    StatusBar1.Panels[5].Text:='fyd = '+floattostrf(ffyd,ffGeneral,5,3)+' kN/cm²';
    StatusBar1.Panels[6].Text:='Es = '+floattostrf(fEs,ffGeneral,5,3)+' kN/cm²';
    StatusBar1.Panels[7].Text:='Deformação Limite do Aço = '+floattostrf(fEpsilonYD,ffGeneral,5,3);
  end;
end;

procedure TFrmMainFrm.MenuItemSaveAsClick(Sender: TObject);
begin
  DlgSave.Title:='Salvar Arquivo como...';
  if DlgSave.Execute then SalvarArquivo(DlgSave.filename);
end;

procedure TFrmMainFrm.SpnEdtArmAtivChange(Sender: TObject);
var
  iContador:integer;

begin
  AtualizarStringGridPolyg(PlanArmAtiv,SpnEdtArmAtiv);
  for iContador:=0 to SpnEdtArmAtiv.Value do begin
    if PlanArmAtiv.Cells[5,iContador]='' then PlanArmAtiv.Cells[5,iContador]:='0';
  end;
end;


procedure TFrmMainFrm.SpnPolygChange(Sender: TObject);

begin
  AtualizarStringGridPolyg(PlanPolyg,SpnPolyg);
end;

procedure TFrmMainFrm.SalvarArquivo(strsave: TFilename);
var
FileToSave:TextFile;
Str1:String;
Real1:Real;
icounter1,icounter2:Integer;
Boolean1:Boolean;
begin
  AssignFile(FileToSave,strsave);
  Rewrite(FileToSave);
  Str1:='Arquivo salvo do OblqCalco';
  WriteLn(FileToSave,Str1);
  WriteLn(FileToSave,ffck);
  WriteLn(FileToSave,fGamaC);
  WriteLn(FileToSave,ffyk);
  WriteLn(FileToSave,fGamaS);
  WriteLn(FileToSave,fEs);
  WriteLn(FileToSave,fEpsilonsu);
  WriteLn(FileToSave,ffpyd);
  WriteLn(FileToSave,ffptd);
  WriteLn(FileToSave,fEp);
  WriteLn(FileToSave,fEpsilonpu);
  WriteLn(FileToSave,fEtaP1);
  WriteLn(FileToSave,fELUalfa);
  WriteLn(FileToSave,fEpsilonErroELU);
  WriteLn(FileToSave,fEpsilonErroMomCurv);
  WriteLn(FileToSave,fEpsilonErroELS);
  WriteLn(FileToSave,fEpsilonELSF);
  icounter1:=fModoDeRupturaDefault;
  WriteLn(FileToSave,icounter1);
  icounter1:=fModoDeRuptura;
  WriteLn(FileToSave,icounter1);
  icounter1:=SpnPolyg.Value;
  WriteLn(FileToSave,icounter1);
  for icounter2:=1 to icounter1 do begin
    Real1:=strtofloat(PlanPolyg.Cells[1,icounter2]);
    WriteLn(FileToSave,Real1);
    Real1:=strtofloat(PlanPolyg.Cells[2,icounter2]);
    WriteLn(FileToSave,Real1);
  end;
  Boolean1:=CheckBoxArmadPasiv.Checked;
  WriteLn(FileToSave,Boolean1);
  if Boolean1 then begin
    icounter1:=SpnEdtArmPas.Value;
    WriteLn(FileToSave,icounter1);
    for icounter2:=1 to icounter1 do begin
        Real1:=strtofloat(PlanArmPas.Cells[1,icounter2]);
        WriteLn(FileToSave,Real1);
        Real1:=strtofloat(PlanArmPas.Cells[2,icounter2]);
        WriteLn(FileToSave,Real1);
        Real1:=strtofloat(PlanArmPas.Cells[3,icounter2]);
        WriteLn(FileToSave,Real1);
    end;
  end;
  Boolean1:=CheckBoxArmadAtiv.Checked;
  WriteLn(FileToSave,Boolean1);
  if Boolean1 then begin
    icounter1:=SpnEdtArmAtiv.Value;
    WriteLn(FileToSave,icounter1);
    for icounter2:=1 to icounter1 do begin
        Real1:=strtofloat(PlanArmAtiv.Cells[1,icounter2]);
        WriteLn(FileToSave,Real1);
        Real1:=strtofloat(PlanArmAtiv.Cells[2,icounter2]);
        WriteLn(FileToSave,Real1);
        Real1:=strtofloat(PlanArmAtiv.Cells[3,icounter2]);
        WriteLn(FileToSave,Real1);
        Real1:=strtofloat(PlanArmAtiv.Cells[4,icounter2]);
        WriteLn(FileToSave,Real1);
        Real1:=strtofloat(PlanArmAtiv.Cells[5,icounter2]);
        WriteLn(FileToSave,Real1);
    end;
  end;
  Str1:=EdtELUNd.Text;
  WriteLn(FileToSave,Str1);
  Str1:=EdtELUMxx.Text;
  WriteLn(FileToSave,Str1);
  Str1:=EdtELUMyy.Text;
  WriteLn(FileToSave,Str1);
  Str1:=EdtMomCurvNd.Text;
  WriteLn(FileToSave,Str1);
  Str1:=EdtELSNd.Text;
  WriteLn(FileToSave,Str1);
  Str1:=EdtELSMxx.Text;
  WriteLn(FileToSave,Str1);
  Str1:=EdtELSMyy.Text;
  WriteLn(FileToSave,Str1);
  CloseFile(FileToSave);
end;

procedure TFrmMainFrm.AbrirArquivo(stropen: TFilename);
var
FileToOpen:TextFile;
Str1:String;
Real1:Real;
icounter1,icounter2:Integer;
boolean1:Boolean;
begin
  AssignFile(FileToOpen,stropen);
  Reset(FileToOpen);
  ReadLn(FileToOpen,Str1);
  if Str1='Arquivo salvo do OblqCalco' then begin
    ReadLn(FileToOpen,Real1);
    ffck:=Real1;
    ReadLn(FileToOpen,Real1);
    fGamaC:=Real1;
    ffcd:=ffck/fGamaC/10;
    ReadLn(FileToOpen,Real1);
    ffyk:=Real1;
    ReadLn(FileToOpen,Real1);
    fGamaS:=Real1;
    ffyd:=ffyk/fGamaS/10;
    ReadLn(FileToOpen,Real1);
    fEs:=Real1;
    ReadLn(FileToOpen,Real1);
    fEpsilonsu:=Real1;
    ReadLn(FileToOpen,Real1);
    ffpyd:=Real1;
    ReadLn(FileToOpen,Real1);
    ffptd:=Real1;
    ReadLn(FileToOpen,Real1);
    fEp:=Real1;
    ReadLn(FileToOpen,Real1);
    fEpsilonpu:=Real1;
    ReadLn(FileToOpen,Real1);
    fEtaP1:=Real1;
    ReadLn(FileToOpen,Real1);
    fELUalfa:=Real1;
    ReadLn(FileToOpen,Real1);
    fEpsilonErroELU:=Real1;
    ReadLn(FileToOpen,Real1);
    fEpsilonErroMomCurv:=Real1;
    ReadLn(FileToOpen,Real1);
    fEpsilonErroELS:=Real1;
    ReadLn(FileToOpen,Real1);
    fEpsilonELSF:=Real1;
    ReadLn(FileToOpen,icounter1);//Modo de ruptura default
    if icounter1=0 then fModoDeRupturaDefault:=0;
    if icounter1=1 then fModoDeRupturaDefault:=1;
    ReadLn(FileToOpen,icounter1);//Modo de ruptura efetivo
    if icounter1=0 then fModoDeRuptura:=0;
    if icounter1=1 then fModoDeRuptura:=1;
    ReadLn(FileToOpen,icounter1);
    SpnPolyg.Value:=icounter1;
    PlanPolyg.RowCount:=icounter1+1;
    for icounter2:=1 to icounter1 do begin
        ReadLn(FileToOpen,Real1);
        PlanPolyg.Cells[1,icounter2]:=floattostr(real1);
        ReadLn(FileToOpen,Real1);
        PlanPolyg.Cells[2,icounter2]:=floattostr(real1);
    end;
    ReadLn(FileToOpen,Str1);//Armadura Passiva
    if Str1='TRUE' then boolean1:=True;
    if Str1='FALSE' then boolean1:=False;
    if boolean1 then begin
      CheckBoxArmadPasiv.Checked:=True;
      ReadLn(FileToOpen,icounter1);
      SpnEdtArmPas.Value:=icounter1;
      PlanArmPas.RowCount:=icounter1+1;
      for icounter2:=1 to icounter1 do begin
          ReadLn(FileToOpen,Real1);
          PlanArmPas.Cells[1,icounter2]:=floattostr(real1);
          ReadLn(FileToOpen,Real1);
          PlanArmPas.Cells[2,icounter2]:=floattostr(real1);
          ReadLn(FileToOpen,Real1);
          PlanArmPas.Cells[3,icounter2]:=floattostr(real1);
      end;
    end;
    if boolean1=False then CheckBoxArmadPasiv.Checked:=False;
    ReadLn(FileToOpen,Str1);//Armadura Ativa
    if Str1='TRUE' then boolean1:=True;
    if Str1='FALSE' then boolean1:=False;
    if boolean1 then begin
      CheckBoxArmadAtiv.Checked:=True;
      ReadLn(FileToOpen,icounter1);
      SpnEdtArmAtiv.Value:=icounter1;
      PlanArmAtiv.RowCount:=icounter1+1;
      for icounter2:=1 to icounter1 do begin
          ReadLn(FileToOpen,Real1);
          PlanArmAtiv.Cells[1,icounter2]:=floattostr(real1);
          ReadLn(FileToOpen,Real1);
          PlanArmAtiv.Cells[2,icounter2]:=floattostr(real1);
          ReadLn(FileToOpen,Real1);
          PlanArmAtiv.Cells[3,icounter2]:=floattostr(real1);
          ReadLn(FileToOpen,Real1);
          PlanArmAtiv.Cells[4,icounter2]:=floattostr(real1);
          ReadLn(FileToOpen,Real1);
          PlanArmAtiv.Cells[5,icounter2]:=floattostr(real1);
      end;
    end;
    if boolean1=false then CheckBoxArmadAtiv.Checked:=False;
    ReadLn(FileToOpen,Real1);
    EdtELUNd.Text:=Floattostr(Real1);
    ReadLn(FileToOpen,Real1);
    EdtELUMxx.Text:=Floattostr(Real1);
    ReadLn(FileToOpen,Real1);
    EdtELUMyy.Text:=Floattostr(Real1);
    ReadLn(FileToOpen,Real1);
    EdtMomCurvNd.Text:=Floattostr(Real1);
    ReadLn(FileToOpen,Real1);
    EdtELSNd.Text:=Floattostr(Real1);
    ReadLn(FileToOpen,Real1);
    EdtELSMxx.Text:=Floattostr(Real1);
    ReadLn(FileToOpen,Real1);
    EdtELSMyy.Text:=Floattostr(Real1);
end;

  CloseFile(FileToOpen);
end;

procedure TFrmMainFrm.SpnEdtArmPasChange(Sender: TObject);
begin
  AtualizarStringGridPolyg(PlanArmPas,SpnEdtArmPas);
end;

procedure TFrmMainFrm.CheckBoxArmadAtivChange(Sender: TObject);
begin
if CheckBoxArmadAtiv.Checked=True then
   begin
     fbArmAtiv:=True;
     LabelArmAtiv.Enabled:=True;
     SpnEdtArmAtiv.Enabled:=True;
     PlanArmAtiv.Enabled:=True;
     PlanArmAtiv.Font.Color:=clBlack;
   end;
if CheckBoxArmadAtiv.Checked=False then
   begin
     fbArmAtiv:=False;
     LabelArmAtiv.Enabled:=False;
     SpnEdtArmAtiv.Enabled:=False;
     PlanArmAtiv.Enabled:=False;
     PlanArmAtiv.Font.Color:=clGray;
   end;
end;

procedure TFrmMainFrm.CheckBoxArmadPasivChange(Sender: TObject);
begin
  if CheckBoxArmadPasiv.Checked=True then begin
    fbArmPas:=True;
    LabelArmPas.Enabled:=True;
    SpnEdtArmPas.Enabled:=True;
    PlanArmPas.Enabled:=True;
    PlanArmPas.Font.Color:=clBlack;
    if fModoDeRupturaDefault=0 then fModoDeRuptura:=0;
    if fModoDeRupturaDefault=1 then fModoDeRuptura:=1;
  end;
  if CheckBoxArmadPasiv.Checked=False then begin
    fbArmPas:=False;
    LabelArmPas.Enabled:=False;
    SpnEdtArmPas.Enabled:=False;
    PlanArmPas.Enabled:=False;
    PlanArmPas.Font.Color:=clGray;
    fModoDeRuptura:=0;
  end;

end;

procedure TFrmMainFrm.ButOpenMatClick(Sender: TObject);
begin
  Materiais.ShowModal;
end;


procedure TFrmMainFrm.ButGoELU(Sender: TObject);
var
iContador:integer;
fpdEpsilC2,fpEpsilonNow,fpEpsilonPrevious,fpNNow,fpNPrevious,fpNAuxiliar,fpEpsilonAuxiliar
  ,fpNretaA,fpNdominio23,fpNdominio45,fpNretaB:real;
LogArquivo: TextFile;

begin
  fVarBin:=True; // Inicio das medidas defensivas
  for iContador:=1 to SpnPolyg.Value do      // Medida defensiva: Tabela de poligonos incompleta
      begin
          if PlanPolyg.Cells[1,iContador]='' then
             begin
               LabelStatusELU.Caption:='Tabela de pontos do poligonos incompleta!';
               fVarBin:=False;
             end;
          if PlanPolyg.Cells[2,iContador]='' then
             begin
               LabelStatusELU.Caption:='Tabela de pontos do poligonos incompleta!';
               fVarBin:=False;
             end;
       end;
  if fbArmPas then
     begin
       for iContador:=1 to SpnEdtArmPas.Value do   // Medida defensiva: Tabela de armadura passiva incompleta
           begin
               if PlanArmPas.Cells[1,iContador]='' then
                  begin
                    LabelStatusELU.Caption:='Tabela das armaduras passivas incompleta!';
                    fVarBin:=False;
                  end;
              if PlanArmPas.Cells[2,iContador]='' then
                 begin
                   LabelStatusELU.Caption:='Tabela das armaduras passivas incompleta!';
                   fVarBin:=False;
                 end;
              if PlanArmPas.Cells[3,iContador]='' then
                 begin
                   LabelStatusELU.Caption:='Tabela das armaduras passivas incompleta!';
                   fVarBin:=False;
                 end;
           end;
     end;
  if fbArmAtiv then
     begin
       for iContador:=1 to SpnEdtArmAtiv.Value do   // Medida defensiva: Tabela de armadura ativa incompleta
          begin
              if PlanArmAtiv.Cells[1,iContador]='' then
                 begin
                   LabelStatusELU.Caption:='Tabela das armaduras ativa incompleta!';
                   fVarBin:=False;
                 end;
              if PlanArmAtiv.Cells[2,iContador]='' then
                 begin
                   LabelStatusELU.Caption:='Tabela das armaduras ativa incompleta!';
                   fVarBin:=False;
                 end;
              if PlanArmAtiv.Cells[3,iContador]='' then
                 begin
                   LabelStatusELU.Caption:='Tabela das armaduras ativa incompleta!';
                   fVarBin:=False;
                 end;
              if PlanArmAtiv.Cells[4,iContador]='' then
                 begin
                   LabelStatusELU.Caption:='Tabela das armaduras ativa incompleta!';
                   fVarBin:=False;
                 end;
          end;
     end;
  if fVarBin=True then
     begin
       SetLength(CoordSec,PlanPolyg.RowCount); // Inicio da aquisição dos dados - Dos Edit para as variaveis
       VetorReceberPontos(PlanPolyg,CoordSec);
       ObterCaractGeomet(CoordSec,fAc,fxcg,fycg); //Calculo das caracteristicas geometricas
       if fbArmPas then
          begin
            SetLength(CoordArmPas,PlanArmPas.RowCount-1);
            SetLength(aAs,PlanArmPas.RowCount-1);
            SetLength(ahsi,PlanArmPas.RowCount-1);
            SetLength(aEpsilonsi,PlanArmPas.RowCount-1);
            VetorReceberPontosArmaduraPassiva(PlanArmPas,aAs,CoordArmPas); // Fim da aquisição dos dados - Dos Edit para as variaveis
            SetLength(CoordArmPasT,Length(CoordArmPas));
            TranslacaoDePontos(CoordArmPas,fxcg,fycg,CoordArmPasT);  //Fim da translação dos pontos
          end;

       if fbArmAtiv then
          begin
            SetLength(CoordArmAtiv,PlanArmAtiv.RowCount-1);
            SetLength(aAp,PlanArmAtiv.RowCount-1);
            SetLength(ahpi,PlanArmAtiv.RowCount-1);
            SetLength(aEpsilonprealong,PlanArmAtiv.RowCount-1);
            SetLength(aEpsilonpsi,PlanArmAtiv.RowCount-1);
            SetLength(aDiametroAp,PlanArmAtiv.RowCount-1);
            VetorReceberPontosArmaduraAtiva(PlanArmAtiv,aAp,aEpsilonprealong,aDiametroAp,CoordArmAtiv); // Fim da aquisição dos dados - Dos Edit para as variaveis
            SetLength(CoordArmAtivT,Length(CoordArmAtiv));
            TranslacaoDePontos(CoordArmAtiv,fxcg,fycg,CoordArmAtivT);  //Fim da translação dos pontos
          end;
       SetLength(CoordSecT,Length(CoordSec));   //Inicio da translação dos pontos
       TranslacaoDePontos(CoordSec,fxcg,fycg,CoordSecT);
       fAs:=0;          //Inicio do calculo dos valores normais
       if fbArmPas then
          begin
              for fTemp:=1 to PlanArmPas.RowCount-1 do
                  begin
                      fAs:=fAs+Strtofloat(PlanArmPas.Cells[3,fTemp]);
                  end;
          end;
       fSigmacd:=ffcd*0.85;
       fNdmax:=(fAc*fSigmacd+fAs*TensaoArmPas(fEpsilonC2,fEs,ffyd));
       fNdmin:=(-ffyd*fAs);

       if fbArmAtiv then
          begin
              for fTemp:=1 to PlanArmAtiv.RowCount-1 do
                  begin
                      varTemp:=(aEpsilonprealong[fTemp-1])+fEpsilonC2;
                      fNdmax:=fNdmax+aAp[fTemp-1]*TensaoArmAtiv(fEpsilonpyd,fEpsilonpu,ffpyd,ffptd,varTemp);
                      varTemp:=(aEpsilonprealong[fTemp-1])+fEpsilonsu;
                      fNdmin:=fNdmin+aAp[fTemp-1]*TensaoArmAtiv(fEpsilonpyd,fEpsilonpu,ffpyd,ffptd,varTemp);
                  end;
          end;
       EdtNmax.Text:=FloatToStrF(fNdmax,ffGeneral,8,6);
       EdtNmin.Text:=FloattostrF(fNdmin,ffGeneral,8,6);      //Fim do calculo dos valores normais
       fNdFixado:=Strtofloat(EdtELUNd.Text);
       if fNdFixado>fNdmax then
          begin
            LabelStatusELU.Caption:='Ruptura por compressão atingida. Força de compressão máxima='+FloatToStrF(fNdmax,ffGeneral,8,6);
            fVarBin:=False;
          end;
       if fNdFixado<fNdmin then
          begin
            LabelStatusELU.Caption:='Ruptura por tração atingida. Força de tração máxima='+FloatToStrF(fNdmin,ffGeneral,8,6);
            fVarBin:=False;
          end;
     end;

  if fVarBin=True then
     begin
       LabelStatusELU.Caption:='Status: Calculando... Aguarde';
       LabelStatusELU.Refresh;


                 // Inicio da preparação para iteração
       falfatemp:=0;
       Serie1.Clear;
       SeriePonto.Clear;
       MemoELU.Clear;
       ProgressBarELU.Position:=0;
       LabelChartX.Caption:='Md,xx (kN cm)';
       LabelChartYSup.Caption:='Md,yy';
       LabelChartYInf.Caption:='(kN.cm)';
       AssignFile(LogArquivo, 'log_normais.txt');
       Rewrite(LogArquivo);
       AbrirLogConcreto;

  repeat
    RotacaoDePontos(falfatemp,CoordSecT,CoordSec);
    if fbArmPas then RotacaoDePontos(falfatemp,CoordArmPasT,CoordArmPas);
    if fbArmAtiv then RotacaoDePontos(falfatemp,CoordArmAtivT,CoordArmAtiv);
    PontosMaximoMinimo(CoordSec,fvmax,fvmin);
    fhalfa:=fvmax-fvmin;
    if fbArmPas then PontosMaximoMinimo(CoordArmPas,fAvmax,fAvmin);
    if fbArmPas then fdalfa:=fvmax-fAvmin;
    if fdalfa<=0.0001 then fModoDeRuptura:=0;
    if fdalfa>0.0001 then begin
       if fModoDeRupturaDefault=1 then begin
         if fbArmPas then begin
           fModoDeRuptura:=1;
         end;
       end;
    end;

    Writeln(LogConcreto, '---INICIO BUSCA DOMINIOS---');

    // PRIMEIRO, CALCULA-SE EM QUAL REGIAO ESTA A FORCA NORMAL DESEJADA
    // N RETA A - LIMITE DOMINIO 1 - Nem se calcula a força no concreto.

    Writeln(LogConcreto, 'Calculando NretaA...');

    fEpsilon1:=fEpsilonsu;
    fEpsilon2:=fEpsilonsu;
    fpNretaA:=0;

    Writeln(LogConcreto, 'eps1= ' + FloatToStr(fEpsilon1));
    Writeln(LogConcreto, 'eps2= ' + FloatToStr(fEpsilon2));
    Writeln(LogConcreto, 'NretaA(conc)= ' + FloatToStr(fpNretaA));

    if fbArmPas then begin
      for fTemp:=0 to Length(CoordArmPasT)-1 do begin // Inicio força do aço
          ahsi[fTemp]:=fvmax-CoordArmPas[fTemp].yi;
          aEpsilonsi[fTemp]:=fEpsilon1-ahsi[fTemp]*(fEpsilon1-fEpsilon2)/fhalfa;
          fpNretaA:=fpNretaA+aAs[fTemp]*TensaoArmPas(aEpsilonsi[fTemp],fEs,ffyd);

          Writeln(LogConcreto, 'y(ap)= ' + FloatToStr(ahsi[fTemp]));
          Writeln(LogConcreto, 'eps(ap)= ' + FloatToStr(aEpsilonsi[fTemp]));
          Writeln(LogConcreto, 'NretaA(conc+ap)= ' + FloatToStr(fpNretaA));
      end; //fim força do aço
    end;

    if fbArmAtiv then begin
      for fTemp:=0 to Length(CoordArmAtivT)-1 do begin // Inicio força do aço
          ahpi[fTemp]:=fvmax-CoordArmAtiv[fTemp].yi;
          aEpsilonpsi[fTemp]:=fEpsilon1-ahpi[fTemp]*(fEpsilon1-fEpsilon2)/fhalfa;
          varTemp:=aEpsilonpsi[fTemp]+aEpsilonprealong[fTemp];
          fpNretaA:=fpNretaA+aAp[fTemp]*TensaoArmAtiv(fEpsilonpyd,fEpsilonpu,ffpyd,ffptd,varTemp);
      end; //fim força do aço
    end;

    Writeln(LogConcreto, 'Calculando Ndominio23...');

    // N DOMINIO 23
    fEpsilon1:=fEpsilonCU;
    fEpsilon2:=fEpsilonsu; // ESSE VALOR DE EPSILON2 É USADO CASO A RUPTURA SEJA NO CONCRETO
    if fModoDeRuptura=1 then fEpsilon2:=fEpsilon1-(fEpsilon1-fEpsilonsu)*fhalfa/fdalfa; // SE FOR NA ARMADURA PASSIVA, ESSE VALOR
    if fEpsilon1=fEpsilon2 then begin
      fxEpsilonC2:=10e6;
      fxalfa:=fxEpsilonC2*fEpsilon1/fEpsilonC2;
    end;
    if fEpsilon1<>fEpsilon2 then begin
      fxalfa:=fhalfa*fEpsilon1/(fEpsilon1-fEpsilon2);  //Explicacoes na secao 4.1.2;
      fxEpsilonC2:=fhalfa*fEpsilonC2/(fEpsilon1-fEpsilon2);  //Explicacoes na secao 4.1.2;
    end;

    fpNdominio23:=ForcaConcreto(CoordSec,ffcd,fxalfa,fxEpsilonC2,fNTensaoConcreto);

    Writeln(LogConcreto, 'eps1= ' + FloatToStr(fEpsilon1));
    Writeln(LogConcreto, 'eps2= ' + FloatToStr(fEpsilon2));
    Writeln(LogConcreto, 'xAlfa= ' + FloatToStr(fxalfa));
    Writeln(LogConcreto, 'xEpc2= ' + FloatToStr(fxEpsilonC2));
    Writeln(LogConcreto, 'Ndominio23(conc)= ' + FloatToStr(fpNdominio23));

    if fbArmPas then begin
      for fTemp:=0 to Length(CoordArmPasT)-1 do begin // Inicio força do aço
          ahsi[fTemp]:=fvmax-CoordArmPas[fTemp].yi;
          aEpsilonsi[fTemp]:=fEpsilon1-ahsi[fTemp]*(fEpsilon1-fEpsilon2)/fhalfa;
          fpNdominio23:=fpNdominio23+aAs[fTemp]*TensaoArmPas(aEpsilonsi[fTemp],fEs,ffyd);
          Writeln(LogConcreto, 'y(ap)= ' + FloatToStr(ahsi[fTemp]));
          Writeln(LogConcreto, 'eps(ap)= ' + FloatToStr(aEpsilonsi[fTemp]));
          Writeln(LogConcreto, 'Ndominio23(conc+ap)= ' + FloatToStr(fpNdominio23));
      end; //fim força do aço
    end;
    if fbArmAtiv then begin
      for fTemp:=0 to Length(CoordArmAtivT)-1 do begin // Inicio força do aço
          ahpi[fTemp]:=fvmax-CoordArmAtiv[fTemp].yi;
          aEpsilonpsi[fTemp]:=fEpsilon1-ahpi[fTemp]*(fEpsilon1-fEpsilon2)/fhalfa;
          varTemp:=aEpsilonpsi[fTemp]+aEpsilonprealong[fTemp];
          fpNdominio23:=fpNdominio23+aAp[fTemp]*TensaoArmAtiv(fEpsilonpyd,fEpsilonpu,ffpyd,ffptd,varTemp);
      end; //fim força do aço
    end;

    Writeln(LogConcreto, 'Calculando Ndominio45...');

    // N DOMINIO 45
    fEpsilon1:=fEpsilonCU;
    fEpsilon2:=0;

    if fEpsilon1=fEpsilon2 then begin
      fxEpsilonC2:=10e6;
      fxalfa:=fxEpsilonC2*fEpsilon1/fEpsilonC2;
    end;
    if fEpsilon1<>fEpsilon2 then begin
      fxalfa:=fhalfa*fEpsilon1/(fEpsilon1-fEpsilon2);
      fxEpsilonC2:=fhalfa*fEpsilonC2/(fEpsilon1-fEpsilon2);  //Explicacoes na secao 4.1.2;
    end;

    fpNdominio45:=ForcaConcreto(CoordSec,ffcd,fxalfa,fxEpsilonC2,fNTensaoConcreto);

    Writeln(LogConcreto, 'eps1= ' + FloatToStr(fEpsilon1));
    Writeln(LogConcreto, 'eps2= ' + FloatToStr(fEpsilon2));
    Writeln(LogConcreto, 'xAlfa= ' + FloatToStr(fxalfa));
    Writeln(LogConcreto, 'xEpc2= ' + FloatToStr(fxEpsilonC2));
    Writeln(LogConcreto, 'Ndominio45(conc)= ' + FloatToStr(fpNdominio45));

    if fbArmPas then
       begin
         for fTemp:=0 to Length(CoordArmPasT)-1 do // Inicio força do aço
             begin
                 ahsi[fTemp]:=fvmax-CoordArmPas[fTemp].yi;
                 aEpsilonsi[fTemp]:=fEpsilon1-ahsi[fTemp]*(fEpsilon1-fEpsilon2)/fhalfa;
                 fpNdominio45:=fpNdominio45+aAs[fTemp]*TensaoArmPas(aEpsilonsi[fTemp],fEs,ffyd);
                 Writeln(LogConcreto, 'y(ap)= ' + FloatToStr(ahsi[fTemp]));
                 Writeln(LogConcreto, 'eps(ap)= ' + FloatToStr(aEpsilonsi[fTemp]));
                 Writeln(LogConcreto, 'Ndominio45(conc+ap)= ' + FloatToStr(fpNdominio45));
             end;
       end; //fim força do aço
    if fbArmAtiv then
       begin
         for fTemp:=0 to Length(CoordArmAtivT)-1 do // Inicio força do aço
             begin
                 ahpi[fTemp]:=fvmax-CoordArmAtiv[fTemp].yi;
                 aEpsilonpsi[fTemp]:=fEpsilon1-ahpi[fTemp]*(fEpsilon1-fEpsilon2)/fhalfa;
                 varTemp:=aEpsilonpsi[fTemp]+aEpsilonprealong[fTemp];
                 fpNdominio45:=fpNdominio45+aAp[fTemp]*TensaoArmAtiv(fEpsilonpyd,fEpsilonpu,ffpyd,ffptd,varTemp);
             end; //fim força do aço
        end;

    Writeln(LogConcreto, 'Calculando NretaB...');

    // N REBA B - LIMITE DOMINIO 5
    fEpsilon1:=fEpsilonC2;
    fEpsilon2:=fEpsilonC2;

    if fEpsilon1=fEpsilon2 then begin
      fxEpsilonC2:=10e6;
      fxalfa:=fxEpsilonC2*fEpsilon1/fEpsilonC2;
    end;
    if fEpsilon1<>fEpsilon2 then begin
      fxalfa:=fhalfa*fEpsilon1/(fEpsilon1-fEpsilon2);
      fxEpsilonC2:=fhalfa*fEpsilonC2/(fEpsilon1-fEpsilon2);  //Explicacoes na secao 4.1.2;
    end;

    fpNretaB:=ForcaConcreto(CoordSec,ffcd,fxalfa,fxEpsilonC2,fNTensaoConcreto);

    Writeln(LogConcreto, 'eps1= ' + FloatToStr(fEpsilon1));
    Writeln(LogConcreto, 'eps2= ' + FloatToStr(fEpsilon2));
    Writeln(LogConcreto, 'xAlfa= ' + FloatToStr(fxalfa));
    Writeln(LogConcreto, 'xEpc2= ' + FloatToStr(fxEpsilonC2));
    Writeln(LogConcreto, 'NretaB(conc)= ' + FloatToStr(fpNretaB));

    if fbArmPas then begin
      for fTemp:=0 to Length(CoordArmPasT)-1 do begin // Inicio força do aço
          ahsi[fTemp]:=fvmax-CoordArmPas[fTemp].yi;
          aEpsilonsi[fTemp]:=fEpsilon1-ahsi[fTemp]*(fEpsilon1-fEpsilon2)/fhalfa;
          fpNretaB:=fpNretaB+aAs[fTemp]*TensaoArmPas(aEpsilonsi[fTemp],fEs,ffyd);
          Writeln(LogConcreto, 'y(ap)= ' + FloatToStr(ahsi[fTemp]));
          Writeln(LogConcreto, 'eps(ap)= ' + FloatToStr(aEpsilonsi[fTemp]));
          Writeln(LogConcreto, 'NretaB(conc+ap)= ' + FloatToStr(fpNretaB));
      end; //fim força do aço
    end;
    if fbArmAtiv then begin
      for fTemp:=0 to Length(CoordArmAtivT)-1 do begin // Inicio força do aço
          ahpi[fTemp]:=fvmax-CoordArmAtiv[fTemp].yi;
          aEpsilonpsi[fTemp]:=fEpsilon1-ahpi[fTemp]*(fEpsilon1-fEpsilon2)/fhalfa;
          varTemp:=aEpsilonpsi[fTemp]+aEpsilonprealong[fTemp];
          fpNretaB:=fpNretaB+aAp[fTemp]*TensaoArmAtiv(fEpsilonpyd,fEpsilonpu,ffpyd,ffptd,varTemp);
      end; //fim força do aço
    end;

    Writeln(LogConcreto, '---FIM BUSCA DOMINIO---');

    if fNdFixado<=fpNretaB then begin
      fVarredura:=3; //Regiao I - Epsilon 2 usado como referencia - pois se Concreto for C90, Epsilon1 é fixo
      fpNPrevious:=fpNdominio45;
      fpEpsilonPrevious:=0;
      fpNNow:=fpNretaB;
      fpEpsilonNow:=fEpsilonC2;
      fpdEpsilC2:=(fEpsilonCU-fEpsilonC2)/fEpsilonCU*fhalfa;
    end;
    if fNdFixado<=fpNdominio45 then begin
      fVarredura:=2; //Regiao II - Epsilon 2 usado como referencia - Epsilon1 assume valor fixo
      fpNPrevious:=fpNdominio23;
      if fModoDeRuptura=0 then fpEpsilonPrevious:=fEpsilonsu;
      if fModoDeRuptura=1 then fpEpsilonPrevious:=fEpsilonCU-(fEpsilonCU-fEpsilonsu)*fhalfa/fdalfa;
      fpNNow:=fpNdominio45;
      fpEpsilonNow:=0;
      fEpsilon1:=fEpsilonCU;
      fEpsilon2:=0;
    end;
    if fNdFixado<=fpNdominio23 then begin
      fVarredura:=1; //Regiao III - Epsilon 1 usado como referencia - Epsilon 2 fixo
      fpNPrevious:=fpNretaA;
      fpEpsilonPrevious:=-fEpsilonYD;
      fpNNow:=fpNdominio23;
      fpEpsilonNow:=fEpsilonCU;
      if fModoDeRuptura=0 then fEpsilon2:=fEpsilonsu;
      if fModoDeRuptura=1 then fEpsilon2:=fEpsilonCU-(fEpsilonCU-fEpsilonsu)*fhalfa/fdalfa;
    end;
    if fNdFixado<=fpNretaA then begin
      fVarredura:=0; //Alem do limite
    end;
    while fVarredura<>0 do begin

        Writeln(LogConcreto, '---INICIO ITERACAO PARA EQUILIBRIO---');

        if fVarredura=3 then begin // Regiao I - Eps2 referencia - Eps1 funcao de Eps2
          fpEpsilonNow:=fpEpsilonNow-(fpEpsilonNow-fpEpsilonPrevious)/(fpNNow-fpNPrevious)*(fpNNow-fNdFixado);
          if fpEpsilonNow<0 then fpEpsilonNow:=0-(0-fpEpsilonPrevious)/(fpNdominio45-fpNPrevious)*(fpNdominio45-fNdFixado);
          if fpEpsilonNow>fEpsilonC2 then fpEpsilonNow:=fEpsilonC2-(fEpsilonC2-fpEpsilonPrevious)/(fpNretaB-fpNPrevious)*(fpNretaB-fNdFixado);
          fpEpsilonPrevious:=fEpsilon2;
          fpNPrevious:=fpNNow;
          fEpsilon2:=fpEpsilonNow;
          fEpsilon1:=(fEpsilonC2*fEpsilonCU-(fEpsilonCU-fEpsilonC2)*fEpsilon2)/fEpsilonC2;
          Writeln(LogArquivo, FloatToStr(falfatemp) + ';' +
                    FloatToStrF(fEpsilon1, ffGeneral, 8, 6) + ';' +
                    FloatToStrF(fEpsilon2, ffGeneral, 8, 6) + ';' +
                    FloatToStrF(fpNNow, ffGeneral, 10, 6));
        end;
        if fVarredura=2 then begin // Regiao II
          fpEpsilonNow:=fpEpsilonNow-(fpEpsilonNow-fpEpsilonPrevious)/(fpNNow-fpNPrevious)*(fpNNow-fNdFixado);
          if fModoDeRuptura=0 then begin
            if fpEpsilonNow<fEpsilonsu then begin
              fpEpsilonNow:=fEpsilonsu;
              fpNNow:=fpNdominio23;
              Writeln(LogArquivo, FloatToStr(falfatemp) + ';' +
                    FloatToStrF(fEpsilon1, ffGeneral, 8, 6) + ';' +
                    FloatToStrF(fEpsilon2, ffGeneral, 8, 6) + ';' +
                    FloatToStrF(fpNNow, ffGeneral, 10, 6));
            end;
          end;
          if fModoDeRuptura=1 then begin
            if fpEpsilonNow<(fEpsilonCU-(fEpsilonCU-fEpsilonsu)*fhalfa/fdalfa) then fpEpsilonNow:=fEpsilonsu-(fEpsilonsu-fpEpsilonPrevious)/(fpNdominio23-fpNPrevious)*(fpNdominio23-fNdFixado);
          end;
          if fpEpsilonNow>0 then fpEpsilonNow:=0-(0-fEpsilon2)/(fpNdominio45-fpNNow)*(fpNdominio45-fNdFixado); // Isto é, se sair do intervalo para um valor maior, compara-se o epsilon com o valor maximo
          fpEpsilonPrevious:=fEpsilon2;
          fpNPrevious:=fpNNow;
          fEpsilon2:=fpEpsilonNow;
          Writeln(LogArquivo, FloatToStr(falfatemp) + ';' +
                    FloatToStrF(fEpsilon1, ffGeneral, 8, 6) + ';' +
                    FloatToStrF(fEpsilon2, ffGeneral, 8, 6) + ';' +
                    FloatToStrF(fpNNow, ffGeneral, 10, 6));
        end;
        if fVarredura=1 then begin // Regiao III
          if fpNNow<=fpNretaA then begin
            fpNPrevious:=fpNdominio23;
            fpEpsilonPrevious:=fEpsilonCU;
          end;
          if abs(fpEpsilonNow-fpEpsilonPrevious)<=0.0000000001 then begin
            fpNPrevious:=fpNdominio23;
            fpEpsilonPrevious:=fEpsilonCU;
          end;
          fpEpsilonNow:=fpEpsilonNow-(fpEpsilonNow-fpEpsilonPrevious)/(fpNNow-fpNPrevious)*(fpNNow-fNdFixado);
          if fpEpsilonNow<fEpsilonsu then fpEpsilonNow:=fEpsilonsu;
          if fpEpsilonNow>fEpsilonCU then fpEpsilonNow:=fEpsilonCU;
          fpEpsilonPrevious:=fEpsilon1;
          fpNPrevious:=fpNNow;
          fEpsilon1:=fpEpsilonNow;
          Writeln(LogArquivo, FloatToStr(falfatemp) + ';' +
                    FloatToStrF(fEpsilon1, ffGeneral, 8, 6) + ';' +
                    FloatToStrF(fEpsilon2, ffGeneral, 8, 6) + ';' +
                    FloatToStrF(fpNNow, ffGeneral, 10, 6));
          if fModoDeRuptura=1 then fEpsilon2:=fEpsilon1-(fEpsilon1-fEpsilonsu)*fhalfa/fdalfa;
          // Caso o modo de ruptura seja no concreto, o valor é constante fEpsilonsu
        end;

        if abs(fpEpsilonNow-fpEpsilonPrevious)<=fEpsilonErroELU then begin
          fVarredura:=0;
        end;


            // Arredondar os epsilon, pois eles estão adquirindo erros (nas casas decimais avançadas
            fStrtemp:=FloattostrF(fEpsilon1,ffGeneral,8,6);
            fEpsilon1:=StrToFloat(fStrtemp);
            fStrtemp:=FloattostrF(fEpsilon2,ffGeneral,8,6);
            fEpsilon2:=StrToFloat(fStrtemp);
            if abs(fEpsilon1)<0.000000001 then fEpsilon1:=0;
            if abs(fEpsilon2)<0.000000001 then fEpsilon2:=0;

            if fEpsilon1=fEpsilon2 then begin
              fxEpsilonC2:=10e6;
              fxalfa:=fxEpsilonC2*fEpsilon1/fEpsilonC2;
            end;
            if fEpsilon1<>fEpsilon2 then begin
              fxalfa:=fhalfa*fEpsilon1/(fEpsilon1-fEpsilon2);
              fxEpsilonC2:=fhalfa*fEpsilonC2/(fEpsilon1-fEpsilon2);  //Explicacoes na secao 4.1.2;
            end;


            fpNNow:=ForcaConcreto(CoordSec,ffcd,fxalfa,fxEpsilonC2,fNTensaoConcreto);

            Writeln(LogConcreto, 'eps1= ' + FloatToStr(fEpsilon1));
            Writeln(LogConcreto, 'eps2= ' + FloatToStr(fEpsilon2));
            Writeln(LogConcreto, 'xAlfa= ' + FloatToStr(fxalfa));
            Writeln(LogConcreto, 'xEpc2= ' + FloatToStr(fxEpsilonC2));
            Writeln(LogConcreto, 'N(conc)= ' + FloatToStr(fpNNow));

            Writeln(LogArquivo, FloatToStr(falfatemp) + ';' +
                    FloatToStrF(fEpsilon1, ffGeneral, 8, 6) + ';' +
                    FloatToStrF(fEpsilon2, ffGeneral, 8, 6) + ';' +
                    FloatToStrF(fpNNow, ffGeneral, 10, 6));

            if fbArmPas then
               begin
                    for fTemp:=0 to Length(CoordArmPasT)-1 do // Inicio força do aço
                        begin
                            ahsi[fTemp]:=fvmax-CoordArmPas[fTemp].yi;
                            aEpsilonsi[fTemp]:=fEpsilon1-ahsi[fTemp]*(fEpsilon1-fEpsilon2)/fhalfa;
                            fpNNow:=fpNNow+aAs[fTemp]*TensaoArmPas(aEpsilonsi[fTemp],fEs,ffyd);

                            Writeln(LogConcreto, 'y(ap)= ' + FloatToStr(ahsi[fTemp]));
                            Writeln(LogConcreto, 'eps(ap)= ' + FloatToStr(aEpsilonsi[fTemp]));
                            Writeln(LogConcreto, 'N(conc+ap)= ' + FloatToStr(fpNNow));
                        end; //fim força do aço
               end;

            if fbArmAtiv then
               begin
                 for fTemp:=0 to Length(CoordArmAtivT)-1 do // Inicio força da armadura passiva
                     begin
                         ahpi[fTemp]:=fvmax-CoordArmAtiv[fTemp].yi;
                         aEpsilonpsi[fTemp]:=fEpsilon1-ahpi[fTemp]*(fEpsilon1-fEpsilon2)/fhalfa;
                         varTemp:=aEpsilonpsi[fTemp]+aEpsilonprealong[fTemp];
                         fpNNow:=fpNNow+aAp[fTemp]*TensaoArmAtiv(fEpsilonpyd,fEpsilonpu,ffpyd,ffptd,varTemp);
                     end; //fim força da armadura passiva
                end;

            Writeln(LogConcreto, 'ang= ' + FloatToStr(falfatemp));

            Writeln(LogConcreto, '--FIM ITERACAO---');

            if fVarredura=0 then // Inicio do calculo dos momentos - se a carga axial N foi encontrada
               begin
                    fMtempx:=0;
                    fMtempy:=0;
                    fMtempx:=MomentoXXConcreto(CoordSec,ffcd,fxalfa,fxEpsilonC2,fNTensaoConcreto);
                    //Primeiro, calculando Mxx
                    fMtempy:=MomentoYYConcreto(CoordSec,ffcd,fxalfa,fxEpsilonC2,fNTensaoConcreto);
                    //Entao, calculando Myy
                    if fbArmPas then
                       begin
                         for fTemp:=0 to Length(CoordArmPas)-1 do // Inicio momento do aço
                             begin
                                 ahsi[fTemp]:=fvmax-CoordArmPas[fTemp].yi;
                                 aEpsilonsi[fTemp]:=fEpsilon1-ahsi[fTemp]*(fEpsilon1-fEpsilon2)/fhalfa;
                                 fvarMtemp:=aAs[fTemp]*TensaoArmPas(aEpsilonsi[fTemp],fEs,ffyd); // Calcula-se a força na barra e ela é salva numa variavel temporaria
                                 fMtempx:=fMtempx+fvarMtemp*CoordArmPas[fTemp].yi;
                                 fMtempy:=fMtempy+fvarMtemp*CoordArmPas[fTemp].xi;
                             end; //fim do momento do aço
                       end;
                    if fbArmAtiv then
                       begin
                         for fTemp:=0 to Length(CoordArmAtivT)-1 do // Inicio força da armadura ativa
                             begin
                                 ahpi[fTemp]:=fvmax-CoordArmAtiv[fTemp].yi;
                                 aEpsilonpsi[fTemp]:=fEpsilon1-ahpi[fTemp]*(fEpsilon1-fEpsilon2)/fhalfa;
                                 varTemp:=aEpsilonpsi[fTemp]+aEpsilonprealong[fTemp];
                                 fvarMtemp:=aAp[fTemp]*TensaoArmAtiv(fEpsilonpyd,fEpsilonpu,ffpyd,ffptd,varTemp); // Calcula-se a força na barra e ela é salva numa variavel temporaria
                                 fMtempx:=fMtempx+fvarMtemp*CoordArmAtiv[fTemp].yi;
                                 fMtempy:=fMtempy+fvarMtemp*CoordArmAtiv[fTemp].xi;
                             end; //fim força da armadura ativa
                       end;

                    if abs(fMtempx)<0.000000001 then fMtempx:=0;
                    if abs(fMtempy)<0.000000001 then fMtempy:=0;

                    fMx:=fMtempx*cos(-falfatemp*3.14159265359/180)+fMtempy*sin(-falfatemp*3.14159265359/180); //Girando de U para X
                    fMy:=-fMtempx*sin(-falfatemp*3.14159265359/180)+fMtempy*cos(-falfatemp*3.14159265359/180); //Girando de V para Y

                    if fModoDeRuptura=0 then MemoELU.Append('Angulo='+floattostr(falfatemp)+'; Eps1='+floattostrf(fEpsilon1,ffGeneral,5,4)+'; Eps2='+floattostrf(fEpsilon2,ffGeneral,5,4)+'; x/h='+floattostrf(fxalfa/fhalfa,ffGeneral,5,4)+'; força='+FloattostrF(fpNNow,ffGeneral,5,4)+'; Mxx = '+FloattostrF(fMx,ffGeneral,5,4)+'; Myy = '+FloattostrF(fMy,ffGeneral,5,4));
                    if fModoDeRuptura=1 then MemoELU.Append('Angulo='+floattostr(falfatemp)+'; Eps1='+floattostrf(fEpsilon1,ffGeneral,5,4)+'; Eps2='+floattostrf(fEpsilon2,ffGeneral,5,4)+'; x/d='+floattostrf(fxalfa/fdalfa,ffGeneral,5,4)+'; força='+FloattostrF(fpNNow,ffGeneral,5,4)+'; Mxx = '+FloattostrF(fMx,ffGeneral,5,4)+'; Myy = '+FloattostrF(fMy,ffGeneral,5,4));

                    Serie1.AddXY(fMx,fMy);

               end; // Fim do calculo dos momentos - se a carga axial N foi encontrada

       end;


    falfatemp:=falfatemp+fELUalfa;
    ProgressBarELU.Position:=trunc(falfatemp*100/360);
 until falfatemp>360;
 FecharLogConcreto;
 CloseFile(LogArquivo);
 SeriePonto.AddXY(StrToFloat(EdtELUMxx.Text),StrToFloat(EdtELUMyy.Text));
 LabelStatusELU.Caption:='Status: Ocioso';
 LabelStatusELU.Refresh;

  end;
end;



procedure TFrmMainFrm.ButGoMomCurvClick(Sender: TObject);
var
iContador:integer;
fpEpsilonNow,fpEpsilonPrevious,fpNNow,fpNPrevious,fpNAuxiliar,fpEpsilonAuxiliar
  ,fpdEpsilC2,fpNmax,fpNmin:real;

begin
  fVarBin:=True; // Inicio das medidas defensivas
  for iContador:=1 to SpnPolyg.Value do      // Medida defensiva: Tabela de poligonos incompleta
      begin
          if PlanPolyg.Cells[1,iContador]='' then
             begin
               LabelStatusELU.Caption:='Tabela de pontos do poligonos incompleta!';
               fVarBin:=False;
             end;
          if PlanPolyg.Cells[2,iContador]='' then
             begin
               LabelStatusELU.Caption:='Tabela de pontos do poligonos incompleta!';
               fVarBin:=False;
             end;
       end;
  if fbArmPas then
     begin
       for iContador:=1 to SpnEdtArmPas.Value do   // Medida defensiva: Tabela de armadura passiva incompleta
           begin
               if PlanArmPas.Cells[1,iContador]='' then
                  begin
                    LabelStatusELU.Caption:='Tabela das armaduras passivas incompleta!';
                    fVarBin:=False;
                  end;
              if PlanArmPas.Cells[2,iContador]='' then
                 begin
                   LabelStatusELU.Caption:='Tabela das armaduras passivas incompleta!';
                   fVarBin:=False;
                 end;
              if PlanArmPas.Cells[3,iContador]='' then
                 begin
                   LabelStatusELU.Caption:='Tabela das armaduras passivas incompleta!';
                   fVarBin:=False;
                 end;
           end;
     end;
  if fbArmAtiv then
     begin
       for iContador:=1 to SpnEdtArmAtiv.Value do   // Medida defensiva: Tabela de armadura ativa incompleta
          begin
              if PlanArmAtiv.Cells[1,iContador]='' then
                 begin
                   LabelStatusELU.Caption:='Tabela das armaduras ativa incompleta!';
                   fVarBin:=False;
                 end;
              if PlanArmAtiv.Cells[2,iContador]='' then
                 begin
                   LabelStatusELU.Caption:='Tabela das armaduras ativa incompleta!';
                   fVarBin:=False;
                 end;
              if PlanArmAtiv.Cells[3,iContador]='' then
                 begin
                   LabelStatusELU.Caption:='Tabela das armaduras ativa incompleta!';
                   fVarBin:=False;
                 end;
              if PlanArmAtiv.Cells[4,iContador]='' then
                 begin
                   LabelStatusELU.Caption:='Tabela das armaduras ativa incompleta!';
                   fVarBin:=False;
                 end;
          end;
     end;


  if fVarBin=True then
     begin
       SetLength(CoordSec,PlanPolyg.RowCount); // Inicio da aquisição dos dados - Dos Edit para as variaveis
       VetorReceberPontos(PlanPolyg,CoordSec);
       ObterCaractGeomet(CoordSec,fAc,fxcg,fycg); //Calculo das caracteristicas geometricas
       if fbArmPas then
          begin
            SetLength(CoordArmPas,PlanArmPas.RowCount-1);
            SetLength(aAs,PlanArmPas.RowCount-1);
            SetLength(ahsi,PlanArmPas.RowCount-1);
            SetLength(aEpsilonsi,PlanArmPas.RowCount-1);
            VetorReceberPontosArmaduraPassiva(PlanArmPas,aAs,CoordArmPas); // Fim da aquisição dos dados - Dos Edit para as variaveis
            SetLength(CoordArmPasT,Length(CoordArmPas));
            TranslacaoDePontos(CoordArmPas,fxcg,fycg,CoordArmPasT);  //Fim da translação dos pontos
          end;

       if fbArmAtiv then
          begin
            SetLength(CoordArmAtiv,PlanArmAtiv.RowCount-1);
            SetLength(aAp,PlanArmAtiv.RowCount-1);
            SetLength(ahpi,PlanArmAtiv.RowCount-1);
            SetLength(aEpsilonprealong,PlanArmAtiv.RowCount-1);
            SetLength(aEpsilonpsi,PlanArmAtiv.RowCount-1);
            SetLength(aDiametroAp,PlanArmAtiv.RowCount-1);
            VetorReceberPontosArmaduraAtiva(PlanArmAtiv,aAp,aEpsilonprealong,aDiametroAp,CoordArmAtiv); // Fim da aquisição dos dados - Dos Edit para as variaveis
            SetLength(CoordArmAtivT,Length(CoordArmAtiv));
            TranslacaoDePontos(CoordArmAtiv,fxcg,fycg,CoordArmAtivT);  //Fim da translação dos pontos
          end;
       SetLength(CoordSecT,Length(CoordSec));   //Inicio da translação dos pontos
       TranslacaoDePontos(CoordSec,fxcg,fycg,CoordSecT);
       fAs:=0;          //Inicio do calculo dos valores normais
       if fbArmPas then
          begin
              for fTemp:=1 to PlanArmPas.RowCount-1 do
                  begin
                      fAs:=fAs+Strtofloat(PlanArmPas.Cells[3,fTemp]);
                  end;
          end;
       fSigmacd:=ffcd*0.85;
       fNdmax:=(fAc*fSigmacd+fAs*TensaoArmPas(fEpsilonC2,fEs,ffyd));
       fNdmin:=(-ffyd*fAs);

       if fbArmAtiv then
          begin
              for fTemp:=1 to PlanArmAtiv.RowCount-1 do
                  begin
                      varTemp:=(aEpsilonprealong[fTemp-1])+fEpsilonC2;
                      fNdmax:=fNdmax+aAp[fTemp-1]*TensaoArmAtiv(fEpsilonpyd,fEpsilonpu,ffpyd,ffptd,varTemp);
                      varTemp:=(aEpsilonprealong[fTemp-1])+fEpsilonsu;
                      fNdmin:=fNdmin+aAp[fTemp-1]*TensaoArmAtiv(fEpsilonpyd,fEpsilonpu,ffpyd,ffptd,varTemp);
                  end;
          end;
       EdtNmax.Text:=FloatToStrF(fNdmax,ffGeneral,8,6);
       EdtNmin.Text:=FloattostrF(fNdmin,ffGeneral,8,6);      //Fim do calculo dos valores normais
       fNdFixado:=Strtofloat(EdtMomCurvNd.Text);
       falfatemp:=Strtofloat(EdtAlfaELS.Text);
       if fNdFixado>fNdmax then
          begin
            LabelStatusELS.Caption:='Ruptura por compressão atingida. Força de compressão máxima='+FloatToStrF(fNdmax,ffGeneral,8,6);
            fVarBin:=False;
          end;
       if fNdFixado<fNdmin then
          begin
            LabelStatusELS.Caption:='Ruptura por tração atingida. Força de tração máxima='+FloatToStrF(fNdmin,ffGeneral,8,6);
            fVarBin:=False;
          end;
     end;

  if fVarBin=True then
     begin
       LabelStatusELS.Caption:='Status: Calculando... Aguarde';
       LabelStatusELS.Refresh;
       RotacaoDePontos(falfatemp,CoordSecT,CoordSec);
       if fbArmPas then RotacaoDePontos(falfatemp,CoordArmPasT,CoordArmPas);
       if fbArmAtiv then RotacaoDePontos(falfatemp,CoordArmAtivT,CoordArmAtiv);
       PontosMaximoMinimo(CoordSec,fvmax,fvmin);
       fhalfa:=fvmax-fvmin;
       if fbArmPas then PontosMaximoMinimo(CoordArmPas,fAvmax,fAvmin);
       if fbArmPas then fdalfa:=fAvmax-fAvmin;

       // Inicio da preparação para iteração
       Serie1.Clear;
       MemoELS.Clear;
       LabelChartX.Caption:='1/r (10/m)';
       LabelChartYSup.Caption:='Md';
       LabelChartYinf.Caption:='(kN.cm)';
       ProgressBarELS.Position:=0;
       fVarredura:=1;
       fDeltaEps:=0;

       repeat
       fCurvatura:=fDeltaEps/fhalfa/10; // = 1/r - unidade = 1/m - cuidado ao manusear curvaturas
       fVarredura:=1;
       // N MINIMO
       fEpsilon2:=fEpsilonsu;
       fEpsilon1:=fEpsilon2+fDeltaEps;

        if fEpsilon1=fEpsilon2 then
           begin
              fxEpsilonC2:=10e6;
              fxalfa:=fxEpsilonC2*fEpsilon1/fEpsilonC2;
           end;
        if fEpsilon1<>fEpsilon2 then
           begin
                fxalfa:=fhalfa*fEpsilon1/(fEpsilon1-fEpsilon2);
                fxEpsilonC2:=fhalfa*fEpsilonC2/(fEpsilon1-fEpsilon2);  //Explicacoes na secao 4.1.2
           end;

        fpNmin:=ForcaConcreto(CoordSec,ffcd,fxalfa,fxEpsilonC2,fNTensaoConcreto);

        if fbArmPas then
           begin
             for fTemp:=0 to Length(CoordArmPasT)-1 do // Inicio força do aço
                 begin
                     ahsi[fTemp]:=fvmax-CoordArmPas[fTemp].yi;
                     aEpsilonsi[fTemp]:=fEpsilon1-ahsi[fTemp]*(fEpsilon1-fEpsilon2)/fhalfa;
                     fpNmin:=fpNmin+aAs[fTemp]*TensaoArmPas(aEpsilonsi[fTemp],fEs,ffyd);
                 end; //fim força do aço
           end;
        if fbArmAtiv then
           begin
             for fTemp:=0 to Length(CoordArmAtivT)-1 do // Inicio força do aço
                 begin
                     ahpi[fTemp]:=fvmax-CoordArmAtiv[fTemp].yi;
                     aEpsilonpsi[fTemp]:=fEpsilon1-ahpi[fTemp]*(fEpsilon1-fEpsilon2)/fhalfa;
                     varTemp:=aEpsilonpsi[fTemp]+aEpsilonprealong[fTemp];
                     fpNmin:=fpNmin+aAp[fTemp]*TensaoArmAtiv(fEpsilonpyd,fEpsilonpu,ffpyd,ffptd,varTemp);
                 end; //fim força do aço
            end;
        fpEpsilonPrevious:=fEpsilon2;
        fpNPrevious:=fpNmin;

        // N MAXIMO
        if fDeltaEps>=fEpsilonCU then
           begin
                fEpsilon1:=fEpsilonCU;
                fEpsilon2:=fEpsilon1-fDeltaEps;
           end;
        if fDeltaEps<fEpsilonCU then
           begin
                fpdEpsilC2:=(fEpsilonCU-fEpsilonC2)/fEpsilonCU*fhalfa; //Pode-se excluir essa linha substituindo seu conteudo na linha seguinte
                fEpsilon1:=fEpsilonC2+fpdEpsilC2*fDeltaEps/fhalfa;
                fEpsilon2:=fEpsilon1-fDeltaEps;
           end;

        if fEpsilon1=fEpsilon2 then
           begin
                fxEpsilonC2:=10e6;
                fxalfa:=fxEpsilonC2*fEpsilon1/fEpsilonC2;
           end;
        if fEpsilon1<>fEpsilon2 then
           begin
                fxalfa:=fhalfa*fEpsilon1/(fEpsilon1-fEpsilon2);
                fxEpsilonC2:=fhalfa*fEpsilonC2/(fEpsilon1-fEpsilon2);  //Explicacoes na secao 4.1.2
           end;

        fpNmax:=ForcaConcreto(CoordSec,ffcd,fxalfa,fxEpsilonC2,fNTensaoConcreto);
        if fbArmPas then
           begin
             for fTemp:=0 to Length(CoordArmPasT)-1 do // Inicio força do aço
                 begin
                     ahsi[fTemp]:=fvmax-CoordArmPas[fTemp].yi;
                     aEpsilonsi[fTemp]:=fEpsilon1-ahsi[fTemp]*(fEpsilon1-fEpsilon2)/fhalfa;
                     fpNmax:=fpNmax+aAs[fTemp]*TensaoArmPas(aEpsilonsi[fTemp],fEs,ffyd);
                 end; //fim força do aço
           end;
        if fbArmAtiv then
           begin
             for fTemp:=0 to Length(CoordArmAtivT)-1 do // Inicio força do aço
                 begin
                     ahpi[fTemp]:=fvmax-CoordArmAtiv[fTemp].yi;
                     aEpsilonpsi[fTemp]:=fEpsilon1-ahpi[fTemp]*(fEpsilon1-fEpsilon2)/fhalfa;
                     varTemp:=aEpsilonpsi[fTemp]+aEpsilonprealong[fTemp];
                     fpNmax:=fpNmax+aAp[fTemp]*TensaoArmAtiv(fEpsilonpyd,fEpsilonpu,ffpyd,ffptd,varTemp);
                 end; //fim força do aço
            end;
    fpEpsilonNow:=fEpsilon2;
    fpNNow:=fpNmax;
    fpEpsilonAuxiliar:=fpEpsilonNow;
    fpNAuxiliar:=fpNNow;

    if fNdFixado>=fpNmax then
       begin
            fVarredura:=3;
       end;

    if fNdFixado<=fpNmin then
       begin
            fVarredura:=3;
       end;


    while fVarredura=1 do
         begin
             fpEpsilonNow:=fpEpsilonNow-(fpEpsilonNow-fpEpsilonPrevious)/(fpNNow-fpNPrevious)*(fpNNow-fNdFixado);
             if fpEpsilonNow>fEpsilonC2 then fpEpsilonNow:=fEpsilonC2;
             fpEpsilonPrevious:=fEpsilon2;
             fpNPrevious:=fpNNow;
             fEpsilon2:=fpEpsilonNow;
             fEpsilon1:=fEpsilon2+fDeltaEps;

             if abs(fpEpsilonNow-fpEpsilonPrevious)<=fEpsilonErroMomCurv then //Erro
               begin
                   fVarredura:=2;
               end;

             if fEpsilon1=fEpsilon2 then
                begin
                     fxEpsilonC2:=10e6;
                     fxalfa:=fxEpsilonC2*fEpsilon1/fEpsilonC2;
                end;
             if fEpsilon1<>fEpsilon2 then
                begin
                     fxalfa:=fhalfa*fEpsilon1/(fEpsilon1-fEpsilon2);
                     fxEpsilonC2:=fhalfa*fEpsilonC2/(fEpsilon1-fEpsilon2);  //Explicacoes na secao 4.1.2
                end;

             // Arredondar os epsilon, pois eles estÃ£o adquirindo erros (nas casas decimais avanÃ§adas
             fStrtemp:=FloattostrF(fEpsilon1,ffGeneral,8,6);
             fEpsilon1:=StrToFloat(fStrtemp);
             fStrtemp:=FloattostrF(fEpsilon2,ffGeneral,8,6);
             fEpsilon2:=StrToFloat(fStrtemp);
             if abs(fEpsilon1)<0.000000001 then fEpsilon1:=0;
             if abs(fEpsilon2)<0.000000001 then fEpsilon2:=0;
             if abs(fDeltaEps)<0.000000001 then fDeltaEps:=0;

             fpNNow:=0;
             fpNNow:=ForcaConcreto(CoordSec,ffcd,fxalfa,fxEpsilonC2,fNTensaoConcreto);
             if fbArmPas then
                begin
                     for fTemp:=0 to Length(CoordArmPasT)-1 do // Inicio força do aço
                         begin
                             ahsi[fTemp]:=fvmax-CoordArmPas[fTemp].yi;
                             aEpsilonsi[fTemp]:=fEpsilon1-ahsi[fTemp]*(fEpsilon1-fEpsilon2)/fhalfa;
                             fpNNow:=fpNNow+aAs[fTemp]*TensaoArmPas(aEpsilonsi[fTemp],fEs,ffyd);
                         end; //fim força do aço
                end;
             if fbArmAtiv then
                begin
                  for fTemp:=0 to Length(CoordArmAtivT)-1 do
                      begin
                          ahpi[fTemp]:=fvmax-CoordArmAtiv[fTemp].yi;
                          aEpsilonpsi[fTemp]:=fEpsilon1-ahpi[fTemp]*(fEpsilon1-fEpsilon2)/fhalfa;
                          varTemp:=aEpsilonpsi[fTemp]+aEpsilonprealong[fTemp];
                          fpNNow:=fpNNow+aAp[fTemp]*TensaoArmAtiv(fEpsilonpyd,fEpsilonpu,ffpyd,ffptd,varTemp);
                      end;
                 end;
             if fpNPrevious<=fpNmin then // Este bloco evita que a forca fique constante (no dominio 1, a regiao em que todas as armaduras estao trancionadas, ha forca constante
                begin
                     if fpNNow<=fpNmin then
                        begin
                             fpNPrevious:=fpNAuxiliar;
                             fpEpsilonPrevious:=fpEpsilonAuxiliar;
                        end;
                end;

             if fVarredura=2 then // Inicio do calculo dos momentos - se a carga axial N foi encontrada
                begin
                    fMtempx:=0;
                    fMtempy:=0;

                    //Primeiro, calculando Mxx
                    fMtempx:=MomentoXXConcreto(CoordSec,ffcd,fxalfa,fxEpsilonC2,fNTensaoConcreto);

                    //Entao, calculando Myy
                    fMtempy:=MomentoYYConcreto(CoordSec,ffcd,fxalfa,fxEpsilonC2,fNTensaoConcreto);

                    if fDeltaEps=0 then   // Essa condicao foi necessaria para evitar um bug dentro do programa
                       begin              // Para entender o processo, basta retirar essas 5 linhas e testar o programa
                         fMtempx:=0;      // Em algumas situacoes de curvatura zero, ainda havera momento resistido pelo concreto
                         fMtempy:=0;      // E isso nao pode ocorrer, pois no centro geométrico o momento de inércia de primeira ordem vale zero.
                       end;


                    if fbArmPas then
                       begin
                         for fTemp:=0 to Length(CoordArmPas)-1 do // Inicio momento do aÃ§o
                             begin
                                 ahsi[fTemp]:=fvmax-CoordArmPas[fTemp].yi;
                                 aEpsilonsi[fTemp]:=fEpsilon1-ahsi[fTemp]*(fEpsilon1-fEpsilon2)/fhalfa;
                                 fvarMtemp:=aAs[fTemp]*TensaoArmPas(aEpsilonsi[fTemp],fEs,ffyd); // Calcula-se a forÃ§a na barra e ela Ã© salva numa variavel temporaria
                                 fMtempx:=fMtempx+fvarMtemp*CoordArmPas[fTemp].yi;
                                 fMtempy:=fMtempy+fvarMtemp*CoordArmPas[fTemp].xi;
                             end; //fim do momento do aÃ§o
                       end;
                     if fbArmAtiv then
                        begin
                          for fTemp:=0 to Length(CoordArmAtivT)-1 do // Inicio força da armadura ativa
                              begin
                                  ahpi[fTemp]:=fvmax-CoordArmAtiv[fTemp].yi;
                                  aEpsilonpsi[fTemp]:=fEpsilon1-ahpi[fTemp]*(fEpsilon1-fEpsilon2)/fhalfa;
                                  varTemp:=aEpsilonpsi[fTemp]+aEpsilonprealong[fTemp];
                                  fvarMtemp:=aAp[fTemp]*TensaoArmAtiv(fEpsilonpyd,fEpsilonpu,ffpyd,ffptd,varTemp); // Calcula-se a força na barra e ela é salva numa variavel temporaria
                                  fMtempx:=fMtempx+fvarMtemp*CoordArmAtiv[fTemp].yi;
                                  fMtempy:=fMtempy+fvarMtemp*CoordArmAtiv[fTemp].xi;
                              end; //fim força da armadura ativa
                         end;

                    fMx:=fMtempx*cos(-falfatemp*3.14159265359/180)+fMtempy*sin(-falfatemp*3.14159265359/180); //Girando de U para X
                    fMy:=-fMtempx*sin(-falfatemp*3.14159265359/180)+fMtempy*cos(-falfatemp*3.14159265359/180); //Girando de V para Y

                    if abs(fMx)<0.000000001 then fMtempx:=0;
                    if abs(fMy)<0.000000001 then fMtempy:=0;
                    if abs(fMy)>0.000000001 then MemoELS.Append('Existe momento residual na direção perpendicular!');


                    MemoELS.Append('Eps1='+FloatToStrF(fEpsilon1,ffGeneral,6,5)+'; Eps2='+FloatToStrF(fEpsilon2,ffGeneral,6,5)+'; força='+FloattostrF(fpNNow,ffGeneral,6,5)+'; Curv.='+FloattostrF(fCurvatura,ffGeneral,6,5)+'; Mxx = '+FloattostrF(fMtempx,ffGeneral,6,5)+'; Myy = '+FloattostrF(fMtempy,ffGeneral,6,5));
                    Serie1.AddXY(fCurvatura,fMx);

                end; // Fim do calculo dos momentos - se a carga axial N foi encontrada


      end;
    fDeltaEps:=fDeltaEps+0.1;

    until fVarredura=3;

    LabelStatusELS.Caption:='Status: Ocioso';
    LabelStatusELS.Refresh;

  end;
end;


procedure TFrmMainFrm.ButProcurarClick(Sender: TObject);
var
iContador:integer;

begin
  fVarBin:=True; // Inicio das medidas defensivas
  for iContador:=1 to SpnPolyg.Value do      // Medida defensiva: Tabela de poligonos incompleta
      begin
          if PlanPolyg.Cells[1,iContador]='' then
             begin
               LabelStatusELU.Caption:='Tabela de pontos do poligonos incompleta!';
               fVarBin:=False;
             end;
          if PlanPolyg.Cells[2,iContador]='' then
             begin
               LabelStatusELU.Caption:='Tabela de pontos do poligonos incompleta!';
               fVarBin:=False;
             end;
       end;
  if fbArmPas then
       begin
         for iContador:=1 to SpnEdtArmPas.Value do   // Medida defensiva: Tabela de armadura passiva incompleta
             begin
                 if PlanArmPas.Cells[1,iContador]='' then
                    begin
                      LabelStatusELU.Caption:='Tabela das armaduras passivas incompleta!';
                      fVarBin:=False;
                    end;
                if PlanArmPas.Cells[2,iContador]='' then
                   begin
                     LabelStatusELU.Caption:='Tabela das armaduras passivas incompleta!';
                     fVarBin:=False;
                   end;
                if PlanArmPas.Cells[3,iContador]='' then
                   begin
                     LabelStatusELU.Caption:='Tabela das armaduras passivas incompleta!';
                     fVarBin:=False;
                   end;
             end;
       end;
  if fbArmAtiv then
     begin
       for iContador:=1 to SpnEdtArmAtiv.Value do   // Medida defensiva: Tabela de armadura ativa incompleta
          begin
              if PlanArmAtiv.Cells[1,iContador]='' then
                 begin
                   LabelStatusELU.Caption:='Tabela das armaduras ativa incompleta!';
                   fVarBin:=False;
                 end;
              if PlanArmAtiv.Cells[2,iContador]='' then
                 begin
                   LabelStatusELU.Caption:='Tabela das armaduras ativa incompleta!';
                   fVarBin:=False;
                 end;
              if PlanArmAtiv.Cells[3,iContador]='' then
                 begin
                   LabelStatusELU.Caption:='Tabela das armaduras ativa incompleta!';
                   fVarBin:=False;
                 end;
              if PlanArmAtiv.Cells[4,iContador]='' then
                 begin
                   LabelStatusELU.Caption:='Tabela das armaduras ativa incompleta!';
                   fVarBin:=False;
                 end;
          end;
     end;

  if fVarBin=True then
     begin
       SetLength(CoordSec,PlanPolyg.RowCount); // Inicio da aquisição dos dados - Dos Edit para as variaveis
       VetorReceberPontos(PlanPolyg,CoordSec);
       ObterCaractGeomet(CoordSec,fAc,fxcg,fycg); //Calculo das caracteristicas geometricas
       SetLength(CoordSecT,Length(CoordSec));   //Inicio da translação dos pontos
       TranslacaoDePontos(CoordSec,fxcg,fycg,CoordSecT);
       if fbArmPas then
          begin
            SetLength(CoordArmPas,PlanArmPas.RowCount-1);
            SetLength(aAs,PlanArmPas.RowCount-1);
            SetLength(ahsi,PlanArmPas.RowCount-1);
            SetLength(aEpsilonsi,PlanArmPas.RowCount-1);
            VetorReceberPontosArmaduraPassiva(PlanArmPas,aAs,CoordArmPas); // Fim da aquisição dos dados - Dos Edit para as variaveis
            SetLength(CoordArmPasT,Length(CoordArmPas));
            TranslacaoDePontos(CoordArmPas,fxcg,fycg,CoordArmPasT);  //Fim da translação dos pontos
          end;

       if fbArmAtiv then
          begin
            SetLength(CoordArmAtiv,PlanArmAtiv.RowCount-1);
            SetLength(aAp,PlanArmAtiv.RowCount-1);
            SetLength(ahpi,PlanArmAtiv.RowCount-1);
            SetLength(aEpsilonprealong,PlanArmAtiv.RowCount-1);
            SetLength(aEpsilonpsi,PlanArmAtiv.RowCount-1);
            SetLength(aDiametroAp,PlanArmAtiv.RowCount-1);
            VetorReceberPontosArmaduraAtiva(PlanArmAtiv,aAp,aEpsilonprealong,aDiametroAp,CoordArmAtiv); // Fim da aquisição dos dados - Dos Edit para as variaveis
            SetLength(CoordArmAtivT,Length(CoordArmAtiv));
            TranslacaoDePontos(CoordArmAtiv,fxcg,fycg,CoordArmAtivT);  //Fim da translação dos pontos
          end;

         fEpsilon1:=StrToFloat(EdtProcEps1.Text); // Este bloco inverte Epsilon1 e Epsilon2 automaticamente caso Eps2 seja maior que Eps1
         fEpsilon2:=StrToFloat(EdtProcEps2.Text);
         if fEpsilon2>fEpsilon1 then begin
           falfatemp:=StrToFloat(EdtProcAlfa.Text);
           falfatemp:=falfatemp+180;
           EdtProcAlfa.Text:=Floattostr(falfatemp);
           EdtProcEps1.Text:=FloatToStr(fEpsilon2);
           EdtProcEps2.Text:=FloatToStr(fEpsilon1);
         end;


       if fVarBin=True then begin
         // Inicio da preparação para iteração
         falfatemp:=StrToFloat(EdtProcAlfa.Text);
         fEpsilon1:=StrToFloat(EdtProcEps1.Text);
         fEpsilon2:=StrToFloat(EdtProcEps2.Text);
         RotacaoDePontos(falfatemp,CoordSecT,CoordSec);
         if fbArmPas then RotacaoDePontos(falfatemp,CoordArmPasT,CoordArmPas);
         if fbArmAtiv then RotacaoDePontos(falfatemp,CoordArmAtivT,CoordArmAtiv);
         PontosMaximoMinimo(CoordSec,fvmax,fvmin);
         fhalfa:=fvmax-fvmin;
         if fbArmPas then PontosMaximoMinimo(CoordArmPas,fAvmax,fAvmin);
         if fbArmPas then fdalfa:=fAvmax-fAvmin;
         fNdtemp:=0;
                 // Arredondar os epsilon, pois eles estão adquirindo erros (nas casas decimais avançadas
                 fStrtemp:=FloattostrF(fEpsilon1,ffGeneral,8,6);
                 fEpsilon1:=StrToFloat(fStrtemp);
                 fStrtemp:=FloattostrF(fEpsilon2,ffGeneral,8,6);
                 fEpsilon2:=StrToFloat(fStrtemp);
                 if abs(fEpsilon1)<0.000000001 then fEpsilon1:=0;
                 if abs(fEpsilon2)<0.000000001 then fEpsilon2:=0;

                 if fEpsilon1=fEpsilon2 then begin
                   fxEpsilonC2:=10e6;
                   fxalfa:=fxEpsilonC2*fEpsilon1/fEpsilonC2;
                 end;
                 if fEpsilon1<>fEpsilon2 then begin
                   fxalfa:=fhalfa*fEpsilon1/(fEpsilon1-fEpsilon2);
                   fxEpsilonC2:=fhalfa*fEpsilonC2/(fEpsilon1-fEpsilon2);  //Explicacoes na secao 4.1.2
                 end;

                 fNdtemp:=ForcaConcreto(CoordSec,ffcd,fxalfa,fxEpsilonC2,fNTensaoConcreto);

                 if fbArmPas then
                    begin
                      for fTemp:=0 to Length(CoordArmPasT)-1 do // Inicio força do aço
                          begin
                              ahsi[fTemp]:=fvmax-CoordArmPas[fTemp].yi;
                              aEpsilonsi[fTemp]:=fEpsilon1-ahsi[fTemp]*(fEpsilon1-fEpsilon2)/fhalfa;
                              fNdtemp:=fNdtemp+aAs[fTemp]*TensaoArmPas(aEpsilonsi[fTemp],fEs,ffyd);
                          end; //fim força do aço
                    end;
                 if fbArmAtiv then
                    begin
                      for fTemp:=0 to Length(CoordArmAtivT)-1 do // Inicio força do aço
                          begin
                              ahpi[fTemp]:=fvmax-CoordArmAtiv[fTemp].yi;
                              aEpsilonpsi[fTemp]:=fEpsilon1-ahpi[fTemp]*(fEpsilon1-fEpsilon2)/fhalfa;
                              varTemp:=aEpsilonpsi[fTemp]+aEpsilonprealong[fTemp];
                              fNdtemp:=fNdtemp+aAp[fTemp]*TensaoArmAtiv(fEpsilonpyd,fEpsilonpu,ffpyd,ffptd,varTemp);
                          end; //fim força do aço
                     end;

                 fMtempx:=0;
                 fMtempy:=0;
                 fMtempx:=MomentoXXConcreto(CoordSec,ffcd,fxalfa,fxEpsilonC2,fNTensaoConcreto);
                 //Primeiro, calculando Mxx
                 fMtempy:=MomentoYYConcreto(CoordSec,ffcd,fxalfa,fxEpsilonC2,fNTensaoConcreto);
                 //Entao, calculando Myy
                 if fbArmPas then
                    begin
                      for fTemp:=0 to Length(CoordArmPas)-1 do // Inicio momento do aço
                          begin
                              ahsi[fTemp]:=fvmax-CoordArmPas[fTemp].yi;
                              aEpsilonsi[fTemp]:=fEpsilon1-ahsi[fTemp]*(fEpsilon1-fEpsilon2)/fhalfa;
                              fvarMtemp:=aAs[fTemp]*TensaoArmPas(aEpsilonsi[fTemp],fEs,ffyd); // Calcula-se a força na barra e ela é salva numa variavel temporaria
                              fMtempx:=fMtempx+fvarMtemp*CoordArmPas[fTemp].yi;
                              fMtempy:=fMtempy+fvarMtemp*CoordArmPas[fTemp].xi;
                          end; //fim do momento do aço
                    end;

                 if fbArmAtiv then
                    begin
                      for fTemp:=0 to Length(CoordArmAtivT)-1 do // Inicio força da armadura ativa
                          begin
                              ahpi[fTemp]:=fvmax-CoordArmAtiv[fTemp].yi;
                              aEpsilonpsi[fTemp]:=fEpsilon1-ahpi[fTemp]*(fEpsilon1-fEpsilon2)/fhalfa;
                              varTemp:=aEpsilonpsi[fTemp]+aEpsilonprealong[fTemp];
                              fvarMtemp:=aAp[fTemp]*TensaoArmAtiv(fEpsilonpyd,fEpsilonpu,ffpyd,ffptd,varTemp); // Calcula-se a força na barra e ela é salva numa variavel temporaria
                              fMtempx:=fMtempx+fvarMtemp*CoordArmAtiv[fTemp].yi;
                              fMtempy:=fMtempy+fvarMtemp*CoordArmAtiv[fTemp].xi;
                          end; //fim força da armadura ativa
                     end;

                             if abs(fMtempx)<0.000001 then fMtempx:=0;
                             if abs(fMtempy)<0.000001 then fMtempy:=0;

                             fMx:=fMtempx*cos(-falfatemp*3.14159265359/180)+fMtempy*sin(-falfatemp*3.14159265359/180); //Girando de U para X
                             fMy:=-fMtempx*sin(-falfatemp*3.14159265359/180)+fMtempy*cos(-falfatemp*3.14159265359/180); //Girando de V para Y

                             if abs(fMx)<0.000001 then fMx:=0;
                             if abs(fMy)<0.000001 then fMy:=0;

                             //Procurando angulo de Mxx e Myy
                             if fMx<>0 then
                                begin
                                     falfatemp:=arctan(fMy/fMx)*180/3.14159265359;
                                end;
                             if fMx<0 then falfatemp:=falfatemp+180;
                             if fMx>0 then
                                begin
                                     if fMy<0 then
                                        begin
                                             falfatemp:=falfatemp+360;
                                        end;
                                end;

                             EdtProcResult.Text:='Força='+FloattostrF(fNdtemp,ffGeneral,6,5)+'; Mxx = '+FloattostrF(fMx,ffGeneral,6,5)+'; Myy = '+FloattostrF(fMy,ffGeneral,6,5)+'; Angulo do Vetor Momento ='+FloatToStrF(falfatemp,ffGeneral,6,5);
       end;
     end;
end;

procedure TFrmMainFrm.ButCalc2Click(Sender: TObject);
var
iContador:integer;
fpMxx,fpMyy,fpN,fpNNow,fpMxNow,fpMyNow,fphx,fphy,fpEpsilonC,fpDeltaEpsX,fpDeltaEpsY,fpEpsilonCPrevious,fpDeltaEpsXPrevious,fpDeltaEpsYPrevious,fpDeltaEpsAlfa,fpCurvatura,fpCurvaturaX,fpCurvaturaY,fpVarTemp1,fpVarTemp2:real;
aJacobn,aJacobInverso:array[1..3,1..3] of real;

begin
     if EdtELSNd.Text='' then
        begin
          LabelStatusProcurar2.Caption:='Dados incompletos';
          fVarBin:=False;
        end;
    if EdtELSMxx.Text='' then
       begin
         LabelStatusProcurar2.Caption:='Dados incompletos';
         fVarBin:=False;
       end;
    if EdtELSMyy.Text='' then
       begin
         LabelStatusProcurar2.Caption:='Dados incompletos';
         fVarBin:=False;
       end;
     fVarBin:=True; // Inicio das medidas defensivas
  for iContador:=1 to SpnPolyg.Value do      // Medida defensiva: Tabela de poligonos incompleta
      begin
          if PlanPolyg.Cells[1,iContador]='' then
             begin
               LabelStatusProcurar2.Caption:='Tabela de pontos do poligonos incompleta!';
               fVarBin:=False;
             end;
          if PlanPolyg.Cells[2,iContador]='' then
             begin
               LabelStatusProcurar2.Caption:='Tabela de pontos do poligonos incompleta!';
               fVarBin:=False;
             end;
       end;
  if fbArmPas then
     begin
       for iContador:=1 to SpnEdtArmPas.Value do   // Medida defensiva: Tabela de armadura passiva incompleta
           begin
               if PlanArmPas.Cells[1,iContador]='' then
                  begin
                    LabelStatusProcurar2.Caption:='Tabela das armaduras passivas incompleta!';
                    fVarBin:=False;
                  end;
              if PlanArmPas.Cells[2,iContador]='' then
                 begin
                   LabelStatusProcurar2.Caption:='Tabela das armaduras passivas incompleta!';
                   fVarBin:=False;
                 end;
              if PlanArmPas.Cells[3,iContador]='' then
                 begin
                   LabelStatusProcurar2.Caption:='Tabela das armaduras passivas incompleta!';
                   fVarBin:=False;
                 end;
           end;
     end;
  if fbArmAtiv then
     begin
       for iContador:=1 to SpnEdtArmAtiv.Value do   // Medida defensiva: Tabela de armadura ativa incompleta
          begin
              if PlanArmAtiv.Cells[1,iContador]='' then
                 begin
                   LabelStatusProcurar2.Caption:='Tabela das armaduras ativa incompleta!';
                   fVarBin:=False;
                 end;
              if PlanArmAtiv.Cells[2,iContador]='' then
                 begin
                   LabelStatusProcurar2.Caption:='Tabela das armaduras ativa incompleta!';
                   fVarBin:=False;
                 end;
              if PlanArmAtiv.Cells[3,iContador]='' then
                 begin
                   LabelStatusProcurar2.Caption:='Tabela das armaduras ativa incompleta!';
                   fVarBin:=False;
                 end;
              if PlanArmAtiv.Cells[4,iContador]='' then
                 begin
                   LabelStatusProcurar2.Caption:='Tabela das armaduras ativa incompleta!';
                   fVarBin:=False;
                 end;
          end;
     end;
  if fVarBin=True then
     begin
       fpN:=strtofloat(EdtELSNd.Text);
       fpMxx:=strtofloat(EdtELSMxx.Text);
       fpMyy:=strtofloat(EdtELSMyy.Text);
       SetLength(CoordSec,PlanPolyg.RowCount); // Inicio da aquisição dos dados - Dos Edit para as variaveis
       VetorReceberPontos(PlanPolyg,CoordSec);
       ObterCaractGeomet(CoordSec,fAc,fxcg,fycg); //Calculo das caracteristicas geometricas
       if fbArmPas then
          begin
            SetLength(CoordArmPas,PlanArmPas.RowCount-1);
            SetLength(aAs,PlanArmPas.RowCount-1);
            SetLength(ahsi,PlanArmPas.RowCount-1);
            SetLength(aEpsilonsi,PlanArmPas.RowCount-1);
            VetorReceberPontosArmaduraPassiva(PlanArmPas,aAs,CoordArmPas); // Fim da aquisição dos dados - Dos Edit para as variaveis
            SetLength(CoordArmPasT,Length(CoordArmPas));
            TranslacaoDePontos(CoordArmPas,fxcg,fycg,CoordArmPasT);  //Fim da translação dos pontos
          end;

       if fbArmAtiv then
          begin
            SetLength(CoordArmAtiv,PlanArmAtiv.RowCount-1);
            SetLength(aAp,PlanArmAtiv.RowCount-1);
            SetLength(ahpi,PlanArmAtiv.RowCount-1);
            SetLength(aEpsilonprealong,PlanArmAtiv.RowCount-1);
            SetLength(aEpsilonpsi,PlanArmAtiv.RowCount-1);
            SetLength(aDiametroAp,PlanArmAtiv.RowCount-1);
            VetorReceberPontosArmaduraAtiva(PlanArmAtiv,aAp,aEpsilonprealong,aDiametroAp,CoordArmAtiv); // Fim da aquisição dos dados - Dos Edit para as variaveis
            SetLength(CoordArmAtivT,Length(CoordArmAtiv));
            TranslacaoDePontos(CoordArmAtiv,fxcg,fycg,CoordArmAtivT);  //Fim da translação dos pontos
          end;
       SetLength(CoordSecT,Length(CoordSec));   //Inicio da translação dos pontos
       TranslacaoDePontos(CoordSec,fxcg,fycg,CoordSecT);
       fAs:=0;          //Inicio do calculo dos valores normais
       if fbArmPas then
          begin
              for fTemp:=1 to PlanArmPas.RowCount-1 do
                  begin
                      fAs:=fAs+Strtofloat(PlanArmPas.Cells[3,fTemp]);
                  end;
          end;
       fSigmacd:=ffcd*0.85;
       fNdmax:=(fAc*fSigmacd+fAs*TensaoArmPas(fEpsilonC2,fEs,ffyd));
       fNdmin:=(-ffyd*fAs);

       if fbArmAtiv then
          begin
              for fTemp:=1 to PlanArmAtiv.RowCount-1 do
                  begin
                      varTemp:=(aEpsilonprealong[fTemp-1])+fEpsilonC2;
                      fNdmax:=fNdmax+aAp[fTemp-1]*TensaoArmAtiv(fEpsilonpyd,fEpsilonpu,ffpyd,ffptd,varTemp);
                      varTemp:=(aEpsilonprealong[fTemp-1])-10;
                      fNdmin:=fNdmin+aAp[fTemp-1]*TensaoArmAtiv(fEpsilonpyd,fEpsilonpu,ffpyd,ffptd,varTemp);
                  end;
          end;
//       EdtNmax.Text:=FloatToStrF(fNdmax,ffGeneral,8,6);
//       EdtNmin.Text:=FloattostrF(fNdmin,ffGeneral,8,6);      //Fim do calculo dos valores normais
       if fpN>=fNdmax then
          begin
            LabelStatusProcurar2.Caption:='Ruptura por compressão atingida. Força de compressão máxima='+FloatToStrF(fNdmax,ffGeneral,8,6);
            fVarBin:=False;
          end;
       if fpN<=fNdmin then
          begin
            LabelStatusProcurar2.Caption:='Ruptura por tração atingida. Força de tração máxima='+FloatToStrF(fNdmin,ffGeneral,8,6);
            fVarBin:=False;
          end;
     end;


          if fVarBin=True then
             begin
                  MemoProcurar2.Clear;
                  RotacaoDePontos(0,CoordSecT,CoordSec);
                  if fbArmPas then RotacaoDePontos(0,CoordArmPasT,CoordArmPas);
                  if fbArmAtiv then RotacaoDePontos(0,CoordArmAtivT,CoordArmAtiv);
                  PontosMaximoMinimo(CoordSec,fpVarTemp1,fpVarTemp2);
                  fphy:=fpVarTemp1-fpVarTemp2;

                  RotacaoDePontos(90,CoordSecT,CoordSec);
                  if fbArmPas then RotacaoDePontos(90,CoordArmPasT,CoordArmPas);
                  if fbArmAtiv then RotacaoDePontos(90,CoordArmAtivT,CoordArmAtiv);
                  PontosMaximoMinimo(CoordSec,fpVarTemp1,fpVarTemp2);
                  fphx:=fpVarTemp1-fpVarTemp2;

                  //Encontrou-se fphy e fphx, agora, calcula-se o jacobiano.
                  for iContador:=1 to 4 do
                      begin
                          //Para isso, calculam-se 4 estados de deformacao:
                          // 1 - falfatemp=0; Epsilon1 = 0.1; Epsilon2 = 0.1
                          // 2 - falfatemp=0; Epsilon1 = 0.2; Epsilon2 = 0.2 (isto e, varia-se o fpEpsilonC em 0.1)
                          // 3 - falfatemp=90; Epsilon1 = 0.2; Epsilon2 = 0.0 (isto e, varia-se o fpCurvaturaY em 0.2)
                          // 4 - falfatemp=0; Epsilon1 = 0.2; Epsilon2 = 0.0 (isto e, varia-se o fpCurvaturaX em 0.2)

                          if iContador=1 then
                             begin
                               falfatemp:=0;
                               fEpsilon1:=0.1;
                               fEpsilon2:=0.1;
                             end;
                          if iContador=2 then
                             begin
                               falfatemp:=0;
                               fEpsilon1:=0.2;
                               fEpsilon2:=0.2;
                             end;
                          if iContador=3 then
                             begin
                               falfatemp:=90;
                               fEpsilon1:=0.2;
                               fEpsilon2:=0.0;
                             end;
                          if iContador=4 then
                             begin
                               falfatemp:=0;
                               fEpsilon1:=0.2;
                               fEpsilon2:=0.0;
                             end;

                          RotacaoDePontos(falfatemp,CoordSecT,CoordSec); //inicio, obtencao de N, Mxx e Myy a partir de falfatemp, fEpsilon1 e fEpsilon2;
                          if fbArmPas then RotacaoDePontos(falfatemp,CoordArmPasT,CoordArmPas);
                          if fbArmAtiv then RotacaoDePontos(falfatemp,CoordArmAtivT,CoordArmAtiv);
                          PontosMaximoMinimo(CoordSec,fvmax,fvmin);
                          fhalfa:=fvmax-fvmin;
                          if abs(fEpsilon1)<0.000000001 then fEpsilon1:=0;
                          if abs(fEpsilon2)<0.000000001 then fEpsilon2:=0;
                          if fEpsilon1=fEpsilon2 then
                             begin
                                  fxEpsilonC2:=10e6;
                                  fxalfa:=fxEpsilonC2*fEpsilon1/fEpsilonC2;
                             end;
                          if fEpsilon1<>fEpsilon2 then
                             begin
                                  fxalfa:=fhalfa*fEpsilon1/(fEpsilon1-fEpsilon2);
                                  fxEpsilonC2:=fhalfa*fEpsilonC2/(fEpsilon1-fEpsilon2);  //Explicacoes na secao 4.1.2
                             end;
                          fNdtemp:=ForcaConcreto(CoordSec,ffcd,fxalfa,fxEpsilonC2,fNTensaoConcreto);
                          fMtempx:=MomentoXXConcreto(CoordSec,ffcd,fxalfa,fxEpsilonC2,fNTensaoConcreto);
                          fMtempy:=MomentoYYConcreto(CoordSec,ffcd,fxalfa,fxEpsilonC2,fNTensaoConcreto);

                          if fbArmPas then
                             begin
                                  for fTemp:=0 to Length(CoordArmPasT)-1 do // Inicio força do aço
                                      begin
                                          ahsi[fTemp]:=fvmax-CoordArmPas[fTemp].yi;
                                          aEpsilonsi[fTemp]:=fEpsilon1-ahsi[fTemp]*(fEpsilon1-fEpsilon2)/fhalfa;
                                          fvarMtemp:=aAs[fTemp]*TensaoArmPas(aEpsilonsi[fTemp],fEs,ffyd); // Calcula-se a força na barra e ela é salva numa variavel temporaria
                                          fNdtemp:=fNdtemp+fvarMtemp;
                                          fMtempx:=fMtempx+fvarMtemp*CoordArmPas[fTemp].yi;
                                          fMtempy:=fMtempy+fvarMtemp*CoordArmPas[fTemp].xi;
                                      end; //fim força do aço
                             end;

                          if fbArmAtiv then
                              begin
                                for fTemp:=0 to Length(CoordArmAtivT)-1 do // Inicio força do aço
                                    begin
                                        ahpi[fTemp]:=fvmax-CoordArmAtiv[fTemp].yi;
                                        aEpsilonpsi[fTemp]:=fEpsilon1-ahpi[fTemp]*(fEpsilon1-fEpsilon2)/fhalfa;
                                        varTemp:=aEpsilonpsi[fTemp]+aEpsilonprealong[fTemp];
                                        fvarMtemp:=aAp[fTemp]*TensaoArmAtiv(fEpsilonpyd,fEpsilonpu,ffpyd,ffptd,varTemp); // Calcula-se a força na barra e ela é salva numa variavel temporaria
                                        fNdtemp:=fNdtemp+fvarMtemp;
                                        fMtempx:=fMtempx+fvarMtemp*CoordArmAtiv[fTemp].yi;
                                        fMtempy:=fMtempy+fvarMtemp*CoordArmAtiv[fTemp].xi;
                                    end; //fim força do aço
                               end;

                          if abs(fMtempx)<0.000001 then fMtempx:=0;
                          if abs(fMtempy)<0.000001 then fMtempy:=0;

                          fMx:=fMtempx*cos(-falfatemp*3.14159265359/180)+fMtempy*sin(-falfatemp*3.14159265359/180); //Girando de U para X
                          fMy:=-fMtempx*sin(-falfatemp*3.14159265359/180)+fMtempy*cos(-falfatemp*3.14159265359/180); //Girando de V para Y

                          if abs(fMx)<0.000001 then fMx:=0;
                          if abs(fMy)<0.000001 then fMy:=0;

                          if iContador=1 then
                              begin
                                aJacobn[1,1]:=-fNdtemp;
                                aJacobn[1,2]:=-fMx;
                                aJacobn[1,3]:=-fMy;
                                aJacobn[2,1]:=-fNdtemp;
                                aJacobn[2,2]:=-fMx;
                                aJacobn[2,3]:=-fMy;
                                aJacobn[3,1]:=-fNdtemp;
                                aJacobn[3,2]:=-fMx;
                                aJacobn[3,3]:=-fMy;
                                fpNNow:=fNdtemp;   // Guardando esses 3 valores, para serem utilizados novamente, ver iContador=4
                                fpMxNow:=fMx;
                                fpMyNow:=fMy;
                              end;
                          if iContador=2 then
                              begin
                                aJacobn[1,1]:=(fNdtemp+aJacobn[1,1])/0.1;
                                aJacobn[1,2]:=(fMx+aJacobn[1,2])/0.1;;
                                aJacobn[1,3]:=(fMy+aJacobn[1,3])/0.1;;
                              end;
                          if iContador=3 then
                              begin
                                aJacobn[2,1]:=(fNdtemp+aJacobn[2,1])/0.2;
                                aJacobn[2,2]:=(fMx+aJacobn[2,2])/0.2;
                                aJacobn[2,3]:=(fMy+aJacobn[2,3])/0.2;
                              end;
                          if iContador=4 then
                              begin
                                aJacobn[3,1]:=(fNdtemp+aJacobn[3,1])/0.2;
                                aJacobn[3,2]:=(fMx+aJacobn[3,2])/0.2;
                                aJacobn[3,3]:=(fMy+aJacobn[3,3])/0.2;
                                // Para comecar a iteracao, preciso comparar os valores iniciais, que foram obtidos no iContador=1, repare que salvei-os nas variaveis fpNNow, fpMxNow e fpMyNow, vou agora devolve-las para liberar as variaveis fNdtemp e as suas analogas para momento, os valores foram salvos no iContador=1
                                fNdtemp:=fpNNow;
                                fMx:=fpMxNow;
                                fMy:=fpMyNow;
                                fpEpsilonC:=0.1;
                                fpDeltaEpsX:=0.0;
                                fpDeltaEpsY:=0.0;
                              end;
                      end;
                  // Agora, pode-se fazer o processo iterativo
                  // O pascal nao faz divisao de matrizes, entao o Jacobiano deve ser invertido antes
                  // O jacobiano invertido sera mantido na variavel Jacoboinv
                  varTemp:=aJacobn[1,1]*(aJacobn[2,2]*aJacobn[3,3]-aJacobn[2,3]*aJacobn[3,2])-aJacobn[1,2]*(aJacobn[3,3]*aJacobn[2,1]-aJacobn[2,3]*aJacobn[3,1])+aJacobn[1,3]*(aJacobn[2,1]*aJacobn[3,2]-aJacobn[2,2]*aJacobn[3,1]);
                  aJacobInverso[1,1]:=(aJacobn[2,2]*aJacobn[3,3]-aJacobn[2,3]*aJacobn[3,2])/varTemp;
                  aJacobInverso[2,1]:=-(aJacobn[2,1]*aJacobn[3,3]-aJacobn[2,3]*aJacobn[3,1])/varTemp;
                  aJacobInverso[3,1]:=(aJacobn[2,1]*aJacobn[3,2]-aJacobn[2,2]*aJacobn[3,1])/varTemp;
                  aJacobInverso[1,2]:=-(aJacobn[1,2]*aJacobn[3,3]-aJacobn[1,3]*aJacobn[3,2])/varTemp;
                  aJacobInverso[2,2]:=(aJacobn[1,1]*aJacobn[3,3]-aJacobn[1,3]*aJacobn[3,1])/varTemp;
                  aJacobInverso[3,2]:=-(aJacobn[1,1]*aJacobn[3,2]-aJacobn[1,2]*aJacobn[3,1])/varTemp;
                  aJacobInverso[1,3]:=(aJacobn[1,2]*aJacobn[2,3]-aJacobn[1,3]*aJacobn[2,2])/varTemp;
                  aJacobInverso[2,3]:=-(aJacobn[1,1]*aJacobn[2,3]-aJacobn[1,3]*aJacobn[2,1])/varTemp;
                  aJacobInverso[3,3]:=(aJacobn[1,1]*aJacobn[2,2]-aJacobn[1,2]*aJacobn[2,1])/varTemp;
                  fpEpsilonCPrevious:=0.1;
                  fpDeltaEpsXPrevious:=0.2;
                  fpDeltaEpsYPrevious:=0.2;
                  iContador:=0; // Agora o iContador ira contar o numero de iteracoes - caso exceder 20000 a iteracao ira parar
                  while abs(fpEpsilonCPrevious+fpDeltaEpsXPrevious+fpDeltaEpsYPrevious)>=fEpsilonErroELS do
                     begin
                         fpNNow:=fNdtemp-fpN;
                         fpMxNow:=fMx-fpMxx;
                         fpMyNow:=fMy-fpMyy;
                         iContador:=iContador+1;
                         //Agora, a divisao de matrizes (produto do inverso, na verdade)
                         fpEpsilonCPrevious:=(aJacobInverso[1,1]*fpNNow+aJacobInverso[1,2]*fpMxNow+aJacobInverso[1,3]*fpMyNow)/2;
                         fpEpsilonC:=fpEpsilonC-fpEpsilonCPrevious;
                         fpDeltaEpsXPrevious:=(aJacobInverso[2,1]*fpNNow+aJacobInverso[2,2]*fpMxNow+aJacobInverso[2,3]*fpMyNow)/2;
                         fpDeltaEpsX:=fpDeltaEpsX-fpDeltaEpsXPrevious;
                         fpDeltaEpsYPrevious:=(aJacobInverso[3,1]*fpNNow+aJacobInverso[3,2]*fpMxNow+aJacobInverso[3,3]*fpMyNow)/2;
                         fpDeltaEpsY:=fpDeltaEpsY-fpDeltaEpsYPrevious;

                         fpCurvaturaX:=fpDeltaEpsX/fphx;
                         fpCurvaturaY:=fpDeltaEpsY/fphy;
                         if fpCurvaturaY<0 then
                            begin
                                 if fpCurvaturaX>0 then falfatemp:=arctan(fpCurvaturaX/fpCurvaturaY)*180/pi+180;
                                 if fpCurvaturaX<0 then falfatemp:=arctan(fpCurvaturaX/fpCurvaturaY)*180/pi+180;
                                 if fpCurvaturaX=0 then falfatemp:=180;
                            end;
                         if fpCurvaturaY>0 then
                            begin
                                 if fpCurvaturaX>0 then falfatemp:=arctan(fpCurvaturaX/fpCurvaturaY)*180/pi;
                                 if fpCurvaturaX<0 then falfatemp:=arctan(fpCurvaturaX/fpCurvaturaY)*180/pi+360;
                                 if abs(fpCurvaturaX)<=0.00001 then falfatemp:=0;
                            end;
                         if abs(fpCurvaturaY)<=0.00001 then
                            begin
                                 if fpCurvaturaX>0 then falfatemp:=90;
                                 if fpCurvaturaX<0 then falfatemp:=270;
                                 if abs(fpCurvaturaX)<=0.00001 then falfatemp:=0;
                            end;
                         fpCurvatura:=sqrt(fpCurvaturaX*fpCurvaturaX+fpCurvaturaY*fpCurvaturaY);
                         RotacaoDePontos(falfatemp,CoordSecT,CoordSec);
                         PontosMaximoMinimo(CoordSec,fvmax,fvmin);
                         fhalfa:=fvmax-fvmin;
                         fpDeltaEpsAlfa:=fpCurvatura*fhalfa;
                         fEpsilon1:=fpEpsilonC+fpDeltaEpsAlfa/2;
                         fEpsilon2:=fpEpsilonC-fpDeltaEpsAlfa/2;


                         //Obtencao N e Mx e My
                         RotacaoDePontos(falfatemp,CoordSecT,CoordSec); //inicio, obtencao de N, Mxx e Myy a partir de falfatemp, fEpsilon1 e fEpsilon2;
                         if fbArmPas then RotacaoDePontos(falfatemp,CoordArmPasT,CoordArmPas);
                         if fbArmAtiv then RotacaoDePontos(falfatemp,CoordArmAtivT,CoordArmAtiv);
                         PontosMaximoMinimo(CoordSec,fvmax,fvmin);
                         fhalfa:=fvmax-fvmin;
                         if abs(fEpsilon1)<0.000000001 then fEpsilon1:=0;
                         if abs(fEpsilon2)<0.000000001 then fEpsilon2:=0;
                         if fEpsilon1=fEpsilon2 then
                            begin
                                 fxEpsilonC2:=10e6;
                                 fxalfa:=fxEpsilonC2*fEpsilon1/fEpsilonC2;
                            end;
                         if fEpsilon1<>fEpsilon2 then
                            begin
                                 fxalfa:=fhalfa*fEpsilon1/(fEpsilon1-fEpsilon2);
                                 fxEpsilonC2:=fhalfa*fEpsilonC2/(fEpsilon1-fEpsilon2);  //Explicacoes na secao 4.1.2
                            end;
                         fNdtemp:=ForcaConcreto(CoordSec,ffcd,fxalfa,fxEpsilonC2,fNTensaoConcreto);
                         fMtempx:=MomentoXXConcreto(CoordSec,ffcd,fxalfa,fxEpsilonC2,fNTensaoConcreto);
                         fMtempy:=MomentoYYConcreto(CoordSec,ffcd,fxalfa,fxEpsilonC2,fNTensaoConcreto);

                         if fbArmPas then
                            begin
                                 for fTemp:=0 to Length(CoordArmPasT)-1 do // Inicio força do aço
                                     begin
                                         ahsi[fTemp]:=fvmax-CoordArmPas[fTemp].yi;
                                         aEpsilonsi[fTemp]:=fEpsilon1-ahsi[fTemp]*(fEpsilon1-fEpsilon2)/fhalfa;
                                         fvarMtemp:=aAs[fTemp]*TensaoArmPas(aEpsilonsi[fTemp],fEs,ffyd); // Calcula-se a força na barra e ela é salva numa variavel temporaria
                                         fNdtemp:=fNdtemp+fvarMtemp;
                                         fMtempx:=fMtempx+fvarMtemp*CoordArmPas[fTemp].yi;
                                         fMtempy:=fMtempy+fvarMtemp*CoordArmPas[fTemp].xi;
                                     end; //fim força do aço
                            end;

                         if fbArmAtiv then
                             begin
                               for fTemp:=0 to Length(CoordArmAtivT)-1 do // Inicio força do aço
                                   begin
                                       ahpi[fTemp]:=fvmax-CoordArmAtiv[fTemp].yi;
                                       aEpsilonpsi[fTemp]:=fEpsilon1-ahpi[fTemp]*(fEpsilon1-fEpsilon2)/fhalfa;
                                       varTemp:=aEpsilonpsi[fTemp]+aEpsilonprealong[fTemp];
                                       fvarMtemp:=aAp[fTemp]*TensaoArmAtiv(fEpsilonpyd,fEpsilonpu,ffpyd,ffptd,varTemp); // Calcula-se a força na barra e ela é salva numa variavel temporaria
                                       fNdtemp:=fNdtemp+fvarMtemp;
                                       fMtempx:=fMtempx+fvarMtemp*CoordArmAtiv[fTemp].yi;
                                       fMtempy:=fMtempy+fvarMtemp*CoordArmAtiv[fTemp].xi;
                                   end; //fim força do aço
                              end;

                         if abs(fMtempx)<0.000001 then fMtempx:=0;
                         if abs(fMtempy)<0.000001 then fMtempy:=0;

                         fMx:=fMtempx*cos(-falfatemp*3.14159265359/180)+fMtempy*sin(-falfatemp*3.14159265359/180); //Girando de U para X
                         fMy:=-fMtempx*sin(-falfatemp*3.14159265359/180)+fMtempy*cos(-falfatemp*3.14159265359/180); //Girando de V para Y

                         if abs(fMx)<0.000001 then fMx:=0;
                         if abs(fMy)<0.000001 then fMy:=0;

                         //Fim da obtencao N Mx e My
                         if iContador=20000 then
                             begin
                               fpEpsilonCPrevious:=0;
                               fpDeltaEpsXPrevious:=0;
                               fpDeltaEpsYPrevious:=0;
                             end;

                     end;


       end;
                 //while acaba aqui
                 fCurvatura:=(fEpsilon1-fEpsilon2)/fhalfa/10;
                 MemoProcurar2.Append('Eps1 = '+FloatToStrF(fEpsilon1,ffGeneral,5,4)+'; Eps2 = '+FloatToStrF(fEpsilon2,ffGeneral,5,4)+'; Angulo = '+FloatToStrF(falfatemp,ffGeneral,5,4)+'; Curv 1/r (1/m) = '+FloattostrF(fCurvatura,ffGeneral,6,4));
                 MemoProcurar2.Append('Numero de iteracoes = '+FloatToStr(iContador));
                 MemoProcurar2.Append('');
                 MemoProcurar2.Append('ESTADO-LIMITE DE SERVIÇO: ABERTURA DE FISSURAS');
                 MemoProcurar2.Append('');

                 //Agora vou incluir as tensões e abertura de fissura em cada armadura
                 if fbArmPas then
                    begin
                      MemoProcurar2.Append('   Abertura de fissuras na armadura passiva:');
                      for fTemp:=0 to Length(CoordArmPas)-1 do // Inicio momento do aço
                          begin
                              ahsi[fTemp]:=fvmax-CoordArmPas[fTemp].yi;
                              aEpsilonsi[fTemp]:=fEpsilon1-ahsi[fTemp]*(fEpsilon1-fEpsilon2)/fhalfa;
                              fvarMtemp:=0;
                              if aAs[fTemp]>0 then fvarMtemp:=exp(ln(aAs[fTemp]*4/3.14159265359)*0.5)*10; //Bitola (mm) - calculada a partir da area
                              fvarMtemp:=fvarMtemp/(12.5*2.25); //Primeiro termo
                              fvarMtemp:=fvarMtemp*TensaoArmPas(aEpsilonsi[fTemp],fEs,ffyd)/fEs; // Segundo termo
                              fvarMtemp:=fvarMtemp*3*TensaoArmPas(aEpsilonsi[fTemp],fEs,ffyd)/(0.1*0.3*exp(ln(ffck)*0.666667)); // Terceiro termo
                              if aEpsilonsi[fTemp]>0 then fvarMtemp:=0;
                              MemoProcurar2.Append('   Barra nr.: '+FloatToStrF(fTemp+1,ffGeneral,6,4)+'; Tensao (KN/cm²): '+floattostrF(TensaoArmPas(aEpsilonsi[fTemp],fEs,ffyd),ffGeneral,6,4)+'; wk,1 (mm) : '+floattostrF(fvarMtemp,ffGeneral,6,4));
                          end; //fim da abertura de fissuras da armadura passiva
                    end;
                 if fbArmPas=false then
                    begin
                      MemoProcurar2.Append('   Nao ha armadura passiva para calculo da abertura de fissura');
                    end;

                 if fbArmAtiv then
                    begin
                      MemoProcurar2.Append('');
                      MemoProcurar2.Append('   Abertura de fissuras na armadura ativa:');
                      for fTemp:=0 to Length(CoordArmAtiv)-1 do
                          begin
                              ahpi[fTemp]:=fvmax-CoordArmAtiv[fTemp].yi;
                              aEpsilonpsi[fTemp]:=fEpsilon1-ahpi[fTemp]*(fEpsilon1-fEpsilon2)/fhalfa;
                              varTemp:=aEpsilonpsi[fTemp]+aEpsilonprealong[fTemp];
                              fvarMtemp:=0;
                              fvarMtemp:=aDiametroAp[fTemp]/(12.5*fEtaP1); //Primeiro termo - Bitola (mm) - deve ser fornecida pelo usuario
                              fvarMtemp:=fvarMtemp*(TensaoArmAtiv(fEpsilonpyd,fEpsilonpu,ffpyd,ffptd,varTemp)-TensaoArmAtiv(fEpsilonpyd,fEpsilonpu,ffpyd,ffptd,aEpsilonprealong[fTemp]))/fEp; // Segundo termo
                              fvarMtemp:=fvarMtemp*3*(TensaoArmAtiv(fEpsilonpyd,fEpsilonpu,ffpyd,ffptd,varTemp)-TensaoArmAtiv(fEpsilonpyd,fEpsilonpu,ffpyd,ffptd,aEpsilonprealong[fTemp]))/(0.1*0.3*exp(ln(ffck)*0.666667)); // Terceiro termo
                              if aEpsilonpsi[fTemp]>0 then fvarMtemp:=0;
                              MemoProcurar2.Append('   Barra nr.: '+FloatToStrF(fTemp+1,ffGeneral,6,4)+'; Tensao (KN/cm²): '+floattostrF((TensaoArmAtiv(fEpsilonpyd,fEpsilonpu,ffpyd,ffptd,varTemp)-TensaoArmAtiv(fEpsilonpyd,fEpsilonpu,ffpyd,ffptd,aEpsilonprealong[fTemp])),ffGeneral,6,4)+'; wk,1 (mm) : '+floattostrF(fvarMtemp,ffGeneral,6,4));
                          end; //fim da abertura de fissuras da armadura passiva
                    end;
                 if fbArmAtiv=false then
                    begin
                      MemoProcurar2.Append('   Nao ha armadura ativa para calculo da abertura de fissura');
                    end;


                 MemoProcurar2.Append('');
                 MemoProcurar2.Append('');
                 MemoProcurar2.Append('ESTADO-LIMITE DE FORMACAO DE FISSURAS');
                 MemoProcurar2.Append('Menor deformacao que ocorre na secao = '+FloatToStrF(fEpsilon2,ffGeneral,5,4));
                 MemoProcurar2.Append('Menor deformacao permitida = '+FloatToStrF(-fEpsilonELSF,ffGeneral,5,4));
                 if fEpsilon2>=-fEpsilonELSF then MemoProcurar2.Append('ELS-F: ATENDIDA.');
                 if fEpsilon2<-fEpsilonELSF then MemoProcurar2.Append('ELS-F: NAO ATENDIDA.');
                 MemoProcurar2.Append('');
                 MemoProcurar2.Append('');
                 MemoProcurar2.Append('ESTADO-LIMITE DE DESCOMPRESSAO');
                 MemoProcurar2.Append('Menor deformacao que ocorre na secao = '+FloatToStrF(fEpsilon2,ffGeneral,5,4));
                 MemoProcurar2.Append('Menor deformacao permitida = 0');
                 if fEpsilon2>=0 then MemoProcurar2.Append('ELS-D: ATENDIDA.');
                 if fEpsilon2<0 then MemoProcurar2.Append('ELS-D: NAO ATENDIDA.');
                 MemoProcurar2.Append('');
                 if iContador=20000 then MemoProcurar2.Clear;
                 if iContador=20000 then MemoProcurar2.Append('Processo travou - resultados desconhecidos');
                 if iContador=20000 then MemoProcurar2.Append('As solicitacoes podem nao atender o ELU!');
end;

end.
