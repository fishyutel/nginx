/*
 * 2016 (C) Fish Yu
 */

#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>
#include <ndk.h>
#define N 6


static char *ngx_http_url_encoder(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);
static ngx_int_t ngx_http_set_var_url_encoder(ngx_http_request_t *r, ngx_str_t *val, ngx_http_variable_value_t *v);

static ngx_command_t ngx_http_url_encoder_commands[] = {
    {
        ngx_string("url_encoder"),
        NGX_HTTP_LOC_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_LIF_CONF|NGX_CONF_TAKE2,
        ngx_http_url_encoder,
        0,
        //NGX_HTTP_LOC_CONF_OFFSET,
        //offsetof(ngx_http_hello_loc_conf_t, hello_string),
        0,
        NULL 
    },
    ngx_null_command
};

ngx_http_module_t ngx_http_url_encoder_module_ctx = {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};
ngx_module_t ngx_http_url_encoder_module = {
    NGX_MODULE_V1,
    &ngx_http_url_encoder_module_ctx,  // module context
    ngx_http_url_encoder_commands,     // module directives
    NGX_HTTP_MODULE,                        // module type
    NULL,                                   // init master
    NULL,                                   // init module
    NULL,                                   // init process
    NULL,                                   // init thread
    NULL,                                   // exit thread
    NULL,                                   // exit process
    NULL,                                   // exit master
    NGX_MODULE_V1_PADDING
};

//url编码
static ngx_int_t
url_encode(ngx_str_t *d_url, ngx_http_variable_value_t *s_url)
{   
    ngx_uint_t i=0,len=0;
    u_char hexchars[] = "0123456789ABCDEF";
    u_char *c=s_url->data;
    u_char *p=malloc(s_url->len*3);
    d_url->data=p;
    
    while (i<s_url->len){
        if (*c == ' ') {
            *p++ = '+';
            len++;
        } else if ((*c < '0' && *c != '-' && *c != '.')
                   ||(*c < 'A' && *c > '9')
                   ||(*c > 'Z' && *c < 'a' && *c != '_')
                   ||(*c > 'z')) {
            *p = '%';
            *(p+1) = hexchars[*c >> 4];
            *(p+2) = hexchars[*c & 15];
            p += 3;
            len +=3;
        } else {
            *p++ = *c;
            len++;
        }
        c++;
        i++;
    }
    d_url->len=len;
    return 0;
}

static char *
ngx_http_url_encoder(ngx_conf_t *cf, ngx_command_t *cmd, void *conf)
{
    ngx_str_t          *value, s[1];
    ndk_set_var_t      filter;
            
    filter.type = NDK_SET_VAR_MULTI_VALUE;
    filter.func = ngx_http_set_var_url_encoder; 
    filter.size = 1;  /*其他参数个数*/

    value = cf->args->elts;//需要保存的参数
    value++;
    s[0] = value[1];
   
    return ndk_set_var_multi_value_core(cf, value, s, &filter);
}


static ngx_int_t
ngx_http_set_var_url_encoder (ngx_http_request_t *r, ngx_str_t *val, ngx_http_variable_value_t *v)
{
    url_encode(val,&v[0]);
    return  NGX_OK;
}
