#!/bin/bash

#  install.sh
#  macRogAuraCore
#
#  Created by Nick on 7/22/20.
#  Copyright © 2020 Nick. All rights reserved.

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null && pwd )"

# Create the directory if not present already
if [[ -d /usr/local/bin ]]; then :; else mkdir -p /usr/local/bin; fi

echo "Installing macRogAuraCore..."

rm /usr/local/bin/macRogAuraCore 2>/dev/null

# Copy to the destination
cp "$DIR/macRogAuraCore" /usr/local/bin/
chmod +x /usr/local/bin/macRogAuraCore
