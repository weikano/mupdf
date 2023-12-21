/**
 * 
    gcc -I/usr/local/include -o fz_bbox_example \
	/home/weikan/git/mupdf-android-fitz/libmupdf/docs/examples/fz_bbox_example.c \
	/usr/local/lib/libmupdf.a \
	/usr/local/lib/libmupdf-third.a \
	-lm
./fz_bbox_example document.pdf
*/



#include <mupdf/fitz.h>

#include <stdio.h>

int main(int argc, char** argv)
{
    char* input = argv[1];

    fz_context *ctx;
    fz_document *doc;

    ctx = fz_new_context(NULL, NULL, FZ_STORE_DEFAULT);
    fz_try(ctx)
    {
        fz_register_document_handlers(ctx);
    }
    fz_catch(ctx)
    {
        fz_drop_context(ctx);
        return -1;
    }

    fz_try(ctx)
    {
        doc = fz_open_document(ctx, input);
    }
    fz_catch(ctx)
    {
        fz_drop_context(ctx);
        return -1;
    }

    fz_try(ctx)
    {
        int pagecount = fz_count_pages(ctx, doc);
        int i;
        for(i=0;i<pagecount;i++)
        {
            printf("begin analyze bbox for page %d\n", i);
            fz_page *page = fz_load_page(ctx, doc, i);
            fz_rect rect = fz_make_rect(0,0,0,0);
            fz_device* device = fz_new_bbox_device(ctx, &rect);
            fz_run_page(ctx, page, device, fz_identity, NULL);
            fz_drop_page(ctx, page);
            fz_close_device(ctx, device);
            fz_drop_device(ctx, device);
            printf("bbox at page %d:(%f,%f,%f,%f)\n", i, rect.x0, rect.y0, rect.x1, rect.y1);
        }
        
    }
    fz_catch(ctx)
    {

    }
    
    
    fz_drop_document(ctx,doc);
    fz_drop_context(ctx);        
    
    return 0;
}