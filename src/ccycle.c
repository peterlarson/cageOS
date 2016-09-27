int main()
{
	int bgColor, textColor;
	enableInterrupts();

	interrupt(0x21, 15, 0, 0, 0);

	/* Cycle colors because it looks cool */
	textColor = 0x09;
    while(1)
    {
    	for(bgColor=0x00; bgColor<=0x07; bgColor+=0x01)
    	{
    		textColor -= 0x01;
    		if(textColor <= 0x00)
    			textColor = 0x09;
			interrupt(0x21, 13, bgColor, textColor, 0);
    	}
    }
}