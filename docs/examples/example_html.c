
#include <mupdf/fitz.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv)
{
    char *input;

    int page_number, page_count;
	fz_context *ctx;
	fz_document *doc;

    input = argv[1];
    ctx = fz_new_context(NULL, NULL, FZ_STORE_UNLIMITED);
    if(!ctx) {
        return -1;
    }
    fz_try(ctx)
    {
        fz_register_document_handlers(ctx);
        doc = fz_open_document(ctx, input);
        page_count = fz_count_pages(ctx, doc);
        int i=0;
        for(i=0;i<page_count;i++) {
            char buffer[100];
            fz_page *page = fz_load_page(ctx, doc, i);
            fz_stext_page *text = fz_new_stext_page(ctx, fz_bound_page(ctx, page));
            fz_device* dev = fz_new_stext_device(ctx, text, NULL);
            fz_run_page(ctx, page, dev, fz_identity ,NULL);
            sprintf(buffer, "reflow_%d.html", i);
            fz_output* output = fz_new_output_with_path(ctx, buffer, 0);
            fz_print_stext_page_as_html(ctx, output, page, i);
            fz_drop_output(ctx, output);
            fz_drop_page(ctx, page);
            fz_drop_device(ctx, dev);
            fz_drop_stext_page(ctx, text);
        }
    }
    fz_catch(ctx)
    {
        fz_drop_context(ctx);
        return -1;
    }
    fz_always(ctx)
    {
        fz_drop_document(ctx, doc);
        fz_drop_context(ctx);
    }

    return 0;
}
