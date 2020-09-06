# -*- coding: utf-8 -*-

import os
import sys
import re
from pprint import pprint

def printHelp():
    print("--help vypise na standardni vystup napovedu skriptu\n"
          "--input=\"filename\" Zadany vstupni soubor nebo adresar se zdrojovym kodem v jazyce C.\n"
          "Je-li zadana cesta k adresari, tak jsou postupne analyzovany vsechny soubory s priponou .h v tomto adresari a jeho podadresarich.\n"
          "Pokud nebude tento parametr zadan, tak se analyzuji vsechny hlavickovr soubory\n"
          "--output=\"filename\" Zadany vystupni soubor ve formatu XML v kodováni UTF-8.\n"
          "Pokud tento parametr neni zadan, tak dojde k vypsani vysledku na standardni vystup.\n"
          "--pretty-xml=k Skript zformatuje vysledny XML dokument tak, ze  kazde nove zanoreni\n"
          "bude odsazeno o k mezer oproti predchozimu a XML hlavicka bude od korenového elementu\n"
          "oddelena znakem noveho radku. Pokud k není zadano, tak se pouzije hodnota 4. Pokud tento\n"
          "parametr nebyl zadan, tak se neodsazuje (ani XML hlavicka od korenového elementu).\n"
          "--no-inline Skript přeskoci funkce deklarovane se specifikatorem inline.\n"
          "--max-par=n Skript bude brat v uvahu pouze funkce, ktere maji n ci mene parametru (n musi byt vzdy zadano).\n"
          "--no-duplicates Pokud se v souboru vyskytne vice funkci se stejnym jmenem (napr.\n"
          "deklarace funkce a pozdeji jeji definice), tak se do vysledneho XML souboru ulozi pouze prvni\n"
          "z nich (uvazujte průchod souborem shora dolu). Pokud tento parametr neni zadan, tak se do\n"
          "vysledneho XML souboru ulozi vsechny vyskyty funkce se stejnym jmenem.\n"
          "--remove-whitespace Pri pouziti tohoto parametru skript v obsahu atributu rettype a\n"
          "type (viz níže) nahradi vsechny vyskyty jinych bilych znaku, nez je mezera (tabulator, novy\n"
          "radek atd.), mezerou a odstrani z nich vsechny prebytecne mezery\n"
          "kazdy parametr je volitelny\n")
    exit(0)

def checkParams(args):                                                               #v cykle kontroluje argumenty,pamata zda li byly predtym zadane
    params = {}                                                                      #a uklada je do slovníku
    params['prettyXml']=False;
    params['noInline']=False
    params['noDuplicates']=False
    params['removeWhite']=False
    params['output']=""
    params['input']=""
    params["maxPar"]=False
    inp=False
    output=False
    for x in range(1,len(args)):
        if(args[x]=='--help'):
            if(len(sys.argv)==2):
                printHelp()
            else:
                print("Zle zadany parametre pouzi --help",file=sys.stderr)
                exit(1)
        elif(args[x][0:8]=="--input="):
            if(inp==False):
                params['input']=args[x][8:]
                if(os.path.isdir(params['input'])):
                    if (params['input'][len(params['input']) - 1] != "/"):           #vlozi / za adresar ak je potreba
                        params['input'] += ("/")
                inp=True
            else:
                print("Zle zadany parametre, prepinac --input zadan vice krat", file=sys.stderr)
                exit(1)
        elif(args[x][0:9]=="--output="):
            if(output==False):
                params['output']=args[x][9:]
                output=True
            else:
                print("Zle zadany parametre, prepinac --output zadan vice krat", file=sys.stderr)
                exit(1)
        elif(args[x][0:12]=="--pretty-xml"):
            if(not params['prettyXml']):
                if not args[x][13:]:
                    params['prettyXml']=4;
                elif(not args[x][13:].isnumeric()):
                    print("zadane k neni cislo", file=sys.stderr)
                    exit(1)
                else:
                    params['prettyXml']=args[x][13:]
            else:
                print("Zle zadany parametre, prepinac --pretty-xml zadan vice krat", file=sys.stderr)
                exit(1)
        elif(args[x][0:11]=="--no-inline"):
            if(params['noInline']==False):
                params['noInline']=True
            else:
                print("Zle zadany parametre, prepinac --no-inline zadan vice krat", file=sys.stderr)
                exit(1)
        elif(args[x][0:10]=="--max-par="):
            if(not params['maxPar']):
                if not args[x][10:]:
                    print("Chybajici N",file = sys.stderr)
                    exit(1)
                elif (not (args[x][10:]).isnumeric()):
                    print("N neni cisloo", file=sys.stderr)
                    exit(1)
                else:
                    params['maxPar']=args[x][10:]
            else:
                print("Zle zadany parametre, prepinac --max-par zadan vice krat", file=sys.stderr)
                exit(1)
        elif(args[x]=="--no-duplicates"):
            if(params['noDuplicates']==False):
                params['noDuplicates']=True
            else:
                print("Zle zadany parametre, prepinac --no-duplicates zadan vice krat", file=sys.stderr)
                exit(1)
        elif(args[x]=="--remove-whitespace"):
            if(params['removeWhite']==False):
                params['removeWhite']=True
            else:
                print("Zle zadany parametre, prepinac --remove-whitespace zadan vice krat", file=sys.stderr)
                exit(1)
        else:
            print("Nepovoleny parametr", file=sys.stderr)
            exit(1)
    return params

def trimTheFile(myFile):                                                             #orezava vstupni soubor
    myFile = re.sub(r"""\/\/.*\\\s.*""","",myFile)                                   #vymaze komentare //   \
    myFile = re.sub('\/\/.*\n',"",myFile,count=0,flags=0)                            #vymaze komentare //
    myFile = re.sub('\/\*(\*(?!\/)|[^*])*\*\/',"",myFile,count=0,flags=0)            #vymaze komentare /* */
    myFile = re.sub('".*"',"",myFile,count=0,flags=0)                                #vymaze stringy ""
    myFile = re.sub('\'.*\'',"",myFile,count=0,flags=0)                              #vymaze stringy ''
    myFile = re.sub(r"#.*(\\\s*.*)*;","", myFile, count=0, flags=0)                  #vymaze #define   \
    myFile = re.sub('#.*',"",myFile,count=0,flags=0)                                 #vymaze #define
    return myFile

def getFunctions(myFile):                                                            #pomoci regexu vrati pole deklaraci
    regex =re.compile("([\w\d]+[\w\s\*\d\[\]]*\([\.{3},\w\s\*\d\[\]]*\))")
    myList = regex.findall(myFile)
    return myList

def remoweWhitespaces(funcList):                                                     #vymaze prebytecne bile znaky
    for x in range(len(funcList)):
        funcList[x] = re.sub("\s{2,}"," ",funcList[x])
        funcList[x] = re.sub("\s*\*\s*","*",funcList[x])
    return funcList

def replaceWhitespaces(funcList):                                                     #prepise prebytecne bile znaky
    for x in range(len(funcList)):
        funcList[x] = re.sub("\s"," ",funcList[x])
    return funcList

def openFile(file):                                                                  #otevre soubor a nacte obsah,pokut ma prava na cteni
    if(os.access(file,os.R_OK)):
        myInput = open(file, 'r')
        myFile = myInput.read()
        return myFile
    else:                                                                            #jinak error
        print("Neexistujici zadany vstupni soubor nebo chyba otevreni zadaneho vstupniho souboru pro cteni")
        exit(2)

def setStdout(params):                                                               #nastavuje stdout
    if (params['output']):
        if(os.path.isdir(params['output'])):                                         #kdyz to je adresar - chyba
            print("Chyba pri pokusu o otevreni noveho nebo existujicího zadaneho vystupniho souboru pro zapis",file=sys.stderr)
            exit(3)
        elif(os.path.isfile(params['output'])):
            if(not os.access(params['output'],os.W_OK)):                             #kdyz je to soubor a nema prava na zapis - chyba
                print("Chyba pri pokusu o otevreni noveho nebo existujicího zadaneho vystupniho souboru pro zapis",file=sys.stderr)
                exit(3)
            sys.stdout = open(params['output'], "w")                                 #jinak ma prava, tak ho prepise
        else:
            try:
                sys.stdout = open(params['output'], 'w')                             #skusi ho vytvorit, a presmeruje do neho stdout
            except IOError:
                print('Chyba pri pokusu o otevreni noveho nebo existujicího zadaneho vystupniho souboru pro zapis',file=sys.stderr)
                exit(3)

def getArgs(funcList):                                                               #vytvori 2D pole s argumentama
    args=[]
    for x in range(len(funcList)):
       args.append(getArg(funcList[x]))
    return args

def getArg(func):                                                                    #vytvori pole argumentu na zaklade zadane deklarade
    args = []
    firstP = func.find("(")
    lastP = func.find(")")
    rawArgs = func[firstP+1:lastP].split(",")                                        #argumenty rozdeli podle carky
    if(rawArgs[0]=="void"):
        return args
    for x in range(len(rawArgs)):
        tokens = rawArgs[x].split()                                                  #jednotlive argumenty rozdeli na tokeny
        if(tokens):
            last = tokens[len(tokens)-1]
        else:
            return args
        if(last[0]=="*"):                                                            #zjisti ci meno argumentu nemalo na zacatku *,a jeho polohu
            pos = rawArgs[x].rfind("*")+1
        else:
            pos = rawArgs[x].rfind(last)
        if(tokens[0].strip()!="..."):
            temp=rawArgs[x][:pos].strip()
            if(last[len(last)-1]=="]"):
                temp+=("[]")
                temp = re.sub(r"""\s\w+[[]""", " [", temp)
            args.append(temp)                                    #do pola ulozi typ argumentu,podla pozice posledniho tokenu
    return args

def getNames(funcList):                                                              #ulozi jmena funkci
    names=[]
    for x in range(len(funcList)):
        tempFunc = funcList[x][:funcList[x].find("(")]  # remove args                #odstrani argumenty
        tokens = tempFunc.split()                                                    #zbytek rozdeli na tokeny
        if(tokens[len(tokens)-1][0]=="*"):                                           #posledni token ulozi do pola,s * nebo bez
            names.append(tokens[len(tokens)-1][1:])
        else:
            names.append(tokens[len(tokens)-1])
    return names

def getRetTypes(funcList):                                                           #zjisti navratove typy
    retTypes = []
    for x in range(len(funcList)):
        tempFunc=funcList[x][:funcList[x].find("(")]                                 #odstrani argumenty
        tokens = tempFunc.split()                                                    #rozdeli na tokeny
        if(tokens):                                                                  #ked tam nejake su zjisti pozici posledniho
            last = tokens[len(tokens)-1]
        else:
            last = ""
        if(last[0]=="*"):
            pos = funcList[x].rfind("*")+1
        else:
            pos = funcList[x].rfind(last)
        retTypes.append(funcList[x][:pos].strip())                                   #posledny token orizne a zbytek ulozi do pole
    return retTypes

def printOut(pretty,funcList,params,names,retTypes,args,file,varArgs,newline):       #vypise obsah jedneho souboru
    for x in range(len(funcList)):
        continueFlag = False
        if params['noInline'] and retTypes[x].find("inline")!=-1:                    #kontroluje inline specifikator
            continue
        if (params['maxPar'] and len(args[x])>int(params['maxPar'])):                #kontroluje --max-par
            continue
        if params['noDuplicates']:  # kontroluje predchozi nazvy deklaraci kvuli duplicite
            for y in range(0, x):
                if names[x] == names[y]:
                    continueFlag = True  # priznak na vyskoceni z 2 cyklu
        if continueFlag:
            continue
        print("%s<function file=\"%s\" name=\"%s\" varargs=\"%s\" rettype=\"%s\">" % (#vypis funkce
        "".join(pretty),file, names[x], varArgs[x], retTypes[x]),end=newline)
        for y in range(len(args[x])):                                                #vypis argumentu
            if (args[x][y]):
                print("%s<param number=\"%s\" type=\"%s\" />" % ("".join(pretty * 2), y+1, args[x][y]),end=newline)
        print("%s</function>" % "".join(pretty),end=newline)

def getVarArgs(funcList):                                                            #zjistuje zda li byly v deklaracich "...",
    varArgs=[]                                                                       #a vrati pole s 'no' nebo 'yes'
    for x in range(len(funcList)):
        varArgs.append("no")
        if(funcList[x].find("...")!=-1):
            varArgs[x]="yes"

    return varArgs

def printStuff(params,pretty,newline):                                               #hlavni funkce vypisuje postupne obsahy souboru
    print("<?xml version=\"1.0\" encoding=\"UTF-8\"?>",end=newline)
    if (os.path.isfile(params['input'])):                                            #kdyz je input subor
        myFile = openFile(params['input'])
        trimmedFile = trimTheFile(myFile)
        trimmedFile = deleteFunctionBodies(list(trimmedFile))
        funcList = getFunctions(trimmedFile)
        funcList = replaceWhitespaces(funcList)
        if(params['removeWhite']):                                                   #kdyz byl zadan remowe-whitespace,prebytecne nahradit mezerami
            funcList = remoweWhitespaces(funcList)
        varArgs=getVarArgs(funcList)
        args=getArgs(funcList)
        names=getNames(funcList)
        retTypes=getRetTypes(funcList)
        file = params['input']
        #if(testParams(pretty,funcList,params,names,retTypes,args,file,varArgs,newline)):
        print("<functions dir=\"\">", end=newline)
        printOut(pretty,funcList,params,names,retTypes,args,file,varArgs,newline)#vypis jednoho souboru
        print("</functions>",end=newline)
    else:
        if (not (params['input'])):                                                  #kdyz je input nezadan
            params['input'] = "."
            print("<functions dir=\"./\">",end=newline)
        elif(os.path.isdir(params['input'])):                                        #kdyz je zadan adresar
            if(params['input'][len(params['input'])-1]!="/"):
                params['input']+=("/")
            print("<functions dir=\"%s\">" % params['input'],end=newline)
        for root, dirs, files in os.walk(params['input']):                           #projde vsechny soubory s priponou .h a vypise obsah
            for file in files:
                if (file.endswith(".h")):
                    myFile = openFile(os.path.join(root, file))
                    trimmed = trimTheFile(myFile)
                    trimmed = deleteFunctionBodies(list(trimmed))
                    funcList = getFunctions(trimmed)
                    funcList = replaceWhitespaces(funcList)
                    if (params['removeWhite']):
                        funcList = remoweWhitespaces(funcList)
                    varArgs = getVarArgs(funcList)
                    args = getArgs(funcList)
                    names = getNames(funcList)
                    retTypes = getRetTypes(funcList)
                    if (funcList):
                        if(params['input']=="./" or params['input']=="."):
                            file=re.sub('\\\\',"/",os.path.join(root, file))[2:]
                        printOut(pretty, funcList, params, names, retTypes, args,file,varArgs,newline)
        print("</functions>",end=newline)

def setPretty(params):                                                               #nastavi odsazeni textu
    if (params['prettyXml'] != False):
        pretty = [" "] * int(params['prettyXml'])
        newline = "\n"
    else:
        pretty = ""
        newline = ""
    return pretty,newline

def checkInput(params):                                                              #kontroluje vstup
    if (not os.path.exists((params['input'])) and params['input'] != ""):
        print("Neexistujici soubor,nebo adresar", file=sys.stderr)
        exit(2)
    if( not os.access(params['input'],os.R_OK)  and params['input'] != ""):
        print("nebo nedostacne prava pro cteni", file=sys.stderr)
        exit(2)

def deleteFunctionBodies(str):                                                        #vymaze tela funkcí
    nest = 0
    for i in range(0, len(str)):
        if (str[i] == "{"):
            nest += 1
        if (str[i] == "}"):
            nest -= 1
        if (nest != 0):
            str[i] = ""
    return ''.join(str)

def testParams(pretty,funcList,params,names,retTypes,args,file,varArgs,newline):
    for x in range(len(funcList)):
        continueFlag = False
        if params['noInline'] and retTypes[x].find("inline") != -1:  # kontroluje inline specifikator
            continue
        if (params['maxPar'] and len(args[x]) > int(params['maxPar'])):  # kontroluje --max-par
            continue
        if params['noDuplicates']:  # kontroluje predchozi nazvy deklaraci kvuli duplicite
            for y in range(0, x):
                if names[x] == names[y]:
                    continueFlag = True  # priznak na vyskoceni z 2 cyklu
        if continueFlag:
            continue
        return True
    return False


#skript skontroluje parametry,nastavy vystup,skontroluje vstup a vypise vysledek
params = checkParams(sys.argv)
setStdout(params)
checkInput(params)
pretty,newline = setPretty(params)

printStuff(params,pretty,newline)
if(params['prettyXml']==False):
    print("")












