#/bin/sh

tesseract ll.game.exp0.tif ll.game.exp0 --psm 7 nobatch box.train
unicharset_extractor ll.game.exp0.box
shapeclustering -F font_properties -U unicharset -O ll.unicharset ll.game.exp0.tr
mftraining -F font_properties -U unicharset -O ll.unicharset ll.game.exp0.tr
cntraining ll.game.exp0.tr
mv normproto ll.normproto
mv inttemp ll.inttemp
mv pffmtable ll.pffmtable
mv shapetable ll.shapetable
combine_tessdata ll
mv ll.traineddata /usr/share/tessdata

