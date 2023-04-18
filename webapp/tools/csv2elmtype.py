#!/usr/bin/env python
import sys
import os
import csv
import argparse

T = 4*' '

def main(infile, outdir):
    print("Generazioni...")

    with open(infile, 'r') as f:
        csvreader = csv.reader(f, delimiter=',', skipinitialspace=True)
        lines = []
        for line in csvreader:
            lines.append(line)

        if len(lines) < 2:
            print('Not enough lines')
            exit(1)

        languages = [x.lstrip() for x in lines[0][1:]]

        if len(lines) < 1:
            print('Not enough languages')
            exit(1)

        tmp = []
        for line in lines[1:]:
            tmp.append([x.lstrip() for x in line])

        translations = tmp

    with open(os.path.join(outdir, 'Translations.elm'), 'w') as elm:
        elm.write(
            '-- Automatically generated file, do not edit\n')
        elm.write(
            'module Translations exposing (Language(..), IntlString(..), translate, languageToInt, languageFromInt)\n')
        elm.write('\n')
        elm.write(f'type Language = {languages[0]}\n')

        for lang in languages[1:]:
            elm.write(f'{T}| {lang}\n')

        elm.write('\n')

        access_functions = []

        elm.write(f'type IntlString = {translations[0][0]}\n')
        for string in translations[1:]:
            elm.write(f'{T}| {string[0]}\n')

        for string in translations:
            function = f'translate{string[0]} : Language -> String\n'
            function += f'translate{string[0]} lang =\n'
            function += f'{T}case lang of\n'
            for i in range(0, len(languages)):
                function += f'{T}{T}{languages[i]} -> "{string[i+1]}"\n'

            access_functions.append(function)

        elm.write('\n')

        elm.write(f'translate : Language -> IntlString -> String\n')
        elm.write(f'translate lang intlstring =\n')
        elm.write(f'{T}case intlstring of\n')

        for string in translations:
            elm.write(f'{T}{T}{string[0]} -> translate{string[0]} lang\n')

        for function in access_functions:
            elm.write('\n')
            elm.write(function)

        elm.write('\n')

        elm.write('languageToInt : Language -> Int\n')
        elm.write('languageToInt lang =\n')
        elm.write(f'{T}case lang of\n')
        for i in range(0, len(languages)):
            elm.write(f'{T}{T}{languages[i]} -> {i}\n')

        elm.write('\n')

        elm.write('languageFromInt : Int -> Language\n')
        elm.write('languageFromInt num =\n')
        elm.write(f'{T}case num of\n')
        for i in range(0, len(languages)):
            elm.write(f'{T}{T}{i} -> {languages[i]}\n')
        elm.write(f'{T}{T}_ -> {languages[0]}')


if __name__ == '__main__':
    parser = argparse.ArgumentParser(
        description="Generazione automatica di traduzioni in Elm")
    parser.add_argument('csv', type=str, nargs='?',
                        help='File .csv con le traduzioni')
    parser.add_argument('-o', '--output', type=str, nargs='?', default='.',
                        help='Cartella dove viene salvato il sorgente generato')
    args = parser.parse_args()

    main(args.csv, args.output)
