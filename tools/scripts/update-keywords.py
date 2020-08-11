#!/usr/bin/env python

# This file is subject to the terms and conditions defined in
# file 'LICENSE.md', which is part of this source code package.

import argparse
import glob
import subprocess
import json
import os
from braceexpand import braceexpand as expand


# ***********************************************************************************
#
# Main function
#
# ***********************************************************************************
def main():
    print('updating keywords')

    # ensure a generator is supplied
    generator = args.root + '/tools/keywords_gen/keywords_populator.py'
    if args.generator:
        generator = args.generator

    # ensure a configuration is supplied
    if not args.config:
        args.config = args.root + '/tools/config/keywords-config.json'
    configpath = os.path.normpath(args.config)

    # ensure existence of that configuration
    if not os.path.exists(configpath):
        raise(Warning("the specified configuration file did not exist ('" + configpath + "')"))

    # load configuration
    with open(configpath, 'r') as fin:
        configuration = json.loads(fin.read())

    # handle groups in configuration
    for name, step in configuration.items():
        print('\nstep:\t', name)

        # expand step specifications
        # if a step contains expansion-params and the pathspec has empty braces '{}' 
        # a new set of specs will be generated from the expanded parameters
        # this will continue until all empty braces from all specs are replaced
        pathspecs = [step['pathspec']]
        destspecs = [step['dest']]

        def expand_specs(specs, params):
            new_specs = []
            for spec in specs:
                new_specs.extend(expand(spec.replace('{}', params, 1)))
            return new_specs

        xp_key = 'expansion-params'
        if xp_key in step:  # continue only if there are valid expansion parameters
            for paramgroup in step[xp_key]:
                pathspecs = expand_specs(pathspecs, paramgroup)
                destspecs = expand_specs(destspecs, paramgroup)
                # todo: verify that this will always produce expansions in the same order

        verboseprint('\tspec: ')
        for spec in pathspecs:
            verboseprint('\t\t', spec)

        verboseprint('\tdest: ')
        for spec in destspecs:
            verboseprint('\t\t', spec)

        try:
            if(len(pathspecs) != len(destspecs)):
                raise(Warning('mismatch length of pathspecs and destspecs for this step'))
        except Warning as w:
            print('warning! ' + str(w))
            print('skipping step')
            continue
        
        print('\tgenerating keywords files')

        for idx, val in enumerate(pathspecs):
            generator_args = ['python', generator, '-p', pathspecs[idx], '-d', destspecs[idx]]
            if args.verbose:
                generator_args.append('-v')
            
            if 'recursive' in step:
                generator_args.append('-r')
            
            subprocess.run(generator_args)

    exit()



# ******************************************************************************
#
# Main program flow
#
# ******************************************************************************
if __name__ == '__main__':

    parser = argparse.ArgumentParser(
        description='Utility to update keyword files for the Apollo3 Arduino core')

    parser.add_argument('-r', '--root', dest='root', default='.', required=False, help='path to the root of the core')
    parser.add_argument('-g', '--generator', dest='generator', required=False, help='path to the generator script')
    parser.add_argument('-c', '--config', dest='config', required=False, help="a json configuration file - if not provided fallbs back to '{root}/tools/config/keywords-config.json'")
    parser.add_argument('-v', '--verbose', default=0, help='enable verbose output', action='store_true')

    args = parser.parse_args()

    # Create print function for verbose output if caller deems it: https://stackoverflow.com/questions/5980042/how-to-implement-the-verbose-or-v-option-into-a-script
    if args.verbose:
        def verboseprint(*args):
            # Print each argument separately so caller doesn't need to
            # stuff everything to be printed into a single string
            for arg in args:
                print(arg, end='', flush=True)
            print()
    else:
        verboseprint = lambda *a: None      # do-nothing function

    def twopartprint(verbosestr, printstr):
        if args.verbose:
            print(verbosestr, end = '')

        print(printstr)

    main()