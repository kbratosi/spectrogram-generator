#include "Decoder.hpp"

Decoder::Decoder() {
  av_format_ctx_ = nullptr;
  av_codec_ctx_ = nullptr;
  audio_stream_index_ = -1;
  swr_ = nullptr;
  av_frame_ = nullptr;
  av_packet_ = nullptr;
}

Decoder::~Decoder() {
  if(av_format_ctx_) {
    avformat_close_input(&av_format_ctx_);
    avformat_free_context(av_format_ctx_);
  }
  if(av_codec_ctx_)
    avcodec_free_context(&av_codec_ctx_);
  if(swr_)
    swr_free(&swr_);
  if(av_frame_)
    av_frame_free(&av_frame_);
  if(av_packet_)
    av_packet_free(&av_packet_);
}

  // Initialize fields and structures necessary in decoding process
  // Includes opening audio file and retrieving data from its header
int Decoder::setup(const char *file_name, const int out_sample_rate) {
  if(initFormatContext(file_name) != 0)
    return -1;

  if(initCodecContext(file_name) != 0)
    return -1;

  if(initSwrContext(out_sample_rate) != 0)
    return -1;

  av_frame_ = av_frame_alloc();
	if (!av_frame_) {
		fprintf(stderr, "Error allocating AVFrame\n");
		return -1;
	}

  av_packet_ = av_packet_alloc();
  if (!av_packet_) {
		fprintf(stderr, "Error allocating AVPacket\n");
		return -1;
	}
  return 0;
}

int Decoder::readFile(sample_fmt **data, int *data_size) {
  while (av_read_frame(av_format_ctx_, av_packet_) >= 0) {
    int response;
    
    // if stream indexes do not match -> skip packet
    if (av_packet_->stream_index != audio_stream_index_) {
      av_packet_unref(av_packet_);
      continue;
    }

    // send packet to decoder
    response = avcodec_send_packet(av_codec_ctx_, av_packet_);
    if (response < 0) {
      fprintf(stderr, "Failed to decode packet: %d\n", response);
      return -1;
    }

    // receive decoded frame
    response = avcodec_receive_frame(av_codec_ctx_, av_frame_);
    if (response == AVERROR(EAGAIN) || response == AVERROR_EOF) {
      av_packet_unref(av_packet_);
      continue;
    } else if (response < 0) {
      fprintf(stderr, "Failed to decode packet: %d\n", response);
      return -1;
    }

    // resample frames
    uint8_t* buffer;
    av_samples_alloc(&buffer, NULL, 1, av_frame_->nb_samples, AV_SAMPLE_FMT_S16, 0);
    int frame_count = swr_convert(swr_, &buffer, av_frame_->nb_samples, (const uint8_t**) av_frame_->data, av_frame_->nb_samples);
    // append resampled frames to data
    *data = (sample_fmt*) realloc(*data, (*data_size + av_frame_->nb_samples) * sizeof(sample_fmt));
    memcpy(*data + *data_size, buffer, frame_count * sizeof(sample_fmt));
    *data_size += frame_count;
    
    // clean up
    av_freep(&buffer);
    av_packet_unref(av_packet_);
  }
  return 0;
}

  // open file using libavcodec, retrieve information from header
int Decoder::initFormatContext(const char *file_name) {
  av_format_ctx_ = avformat_alloc_context();
  if (!av_format_ctx_) {
    fprintf(stderr, "Couldn't create AVFormatContext\n");
    return -1;
  }
  
  if(avformat_open_input(&av_format_ctx_, file_name, nullptr, nullptr) != 0) {
    fprintf(stderr, "Could not open file '%s'\n", file_name);
    return -1;
  }
  
  // retrieve stream information from file header
  if(avformat_find_stream_info(av_format_ctx_, nullptr) < 0) {
    fprintf(stderr, "Could not retrieve stream information from '%s'\n", file_name);
    return -1;
  }
  return 0;
}

  // find and open required codec for file decoding
int Decoder::initCodecContext(const char *file_name) {
  AVCodecParameters* av_codec_params;
  AVCodec* av_codec;

  // find stream containing audio packets, save its index
  for(uint i = 0; i < av_format_ctx_->nb_streams; ++i) {
    av_codec_params = av_format_ctx_->streams[i]->codecpar;
    av_codec = avcodec_find_decoder(av_codec_params->codec_id);
    if (!av_codec) {
        continue;
    }
    if (av_codec_params->codec_type == AVMEDIA_TYPE_AUDIO) {
      audio_stream_index_ = i;
      break;
    }
  }
  if (audio_stream_index_ == -1) {
    fprintf(stderr, "Could not retrieve audio stream from file '%s'\n", file_name);
    return -1;
  }

  // find & open codec
  av_codec_ctx_ = avcodec_alloc_context3(av_codec);
  if (!av_codec_ctx_) {
    fprintf(stderr, "Failed to create AVCodecContext" );
    return -1;
  }

  if (avcodec_parameters_to_context(av_codec_ctx_, av_codec_params) < 0) {
    fprintf(stderr, "Couldn't initialize AVCodecContext\n");
    return -1;
  }

  if (avcodec_open2(av_codec_ctx_, av_codec, NULL) < 0) {
    fprintf(stderr, "Couldn't open codec\n");
    return -1;
  }
  return 0;
}

  // initialize and setup resampler
int Decoder::initSwrContext(const int out_sample_rate) {
  swr_ = swr_alloc();
  av_opt_set_int(swr_, "in_channel_count",  av_codec_ctx_->channels, 0);
  av_opt_set_int(swr_, "out_channel_count", 1, 0);
  av_opt_set_int(swr_, "in_channel_layout",  av_codec_ctx_->channel_layout, 0);
  av_opt_set_int(swr_, "out_channel_layout", AV_CH_LAYOUT_MONO, 0);
  av_opt_set_int(swr_, "in_sample_rate", av_codec_ctx_->sample_rate, 0);
  av_opt_set_int(swr_, "out_sample_rate", out_sample_rate, 0);
  av_opt_set_sample_fmt(swr_, "in_sample_fmt",  av_codec_ctx_->sample_fmt, 0);
  av_opt_set_sample_fmt(swr_, "out_sample_fmt", AV_SAMPLE_FMT_S16,  0);
  swr_init(swr_);
  if (!swr_is_initialized(swr_)) {
    fprintf(stderr, "Resampler has not been properly initialized\n");
    return -1;
  }
  return 0;
}